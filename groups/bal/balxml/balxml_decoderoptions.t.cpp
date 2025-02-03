// balxml_decoderoptions.t.cpp                                        -*-C++-*-

#include <balxml_decoderoptions.h>

#include <bdlat_formattingmode.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_climits.h>
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
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: `setAttributeName` and `attributeName`.
//
// Primary Manipulators:
//  - `setMaxDepth`
//  - `setFormattingMode`
//  - `setSkipUnknownElements`
//  - `setValidateInputIsUtf8`
//  - `setValidateRootTag`
//
// Basic Accessors:
//  - `maxDepth`
//  - `formattingMode`
//  - `skipUnknownElements`
//  - `validateInputIsUtf8`
//  - `validateRootTag`
//
// Certain standard value-semantic-type test cases are omitted:
//  - [ 8] -- `swap` is not implemented for this class.
//  - [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//  - ACCESSOR methods are declared `const`.
//  - Precondition violations are detected in appropriate build modes.
//  - No memory is allocated from any allocator.
//
// Global Assumptions:
//  - ACCESSOR methods are `const` thread-safe.
//  - Individual attribute types are presumed to be *alias-safe*; hence, only
//    certain methods require the testing of this property:
//    - copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] balxml::DecoderOptions();
// [ 7] balxml::DecoderOptions(const balxml::DecoderOptions& original);
// [ 2] ~balxml::DecoderOptions();
//
// MANIPULATORS
// [ 9] operator=(const balxml::DecoderOptions& rhs);
// [ 3] setMaxDepth(int value);
// [ 3] setFormattingMode(int value);
// [ 3] setSkipUnknownElements(bool value);
// [ 3] setValidateInputIsUtf8(bool value);
// [ 3] setValidateRootTag(bool value);
//
// ACCESSORS
// [ 4] int  maxDepth() const;
// [ 4] int  formattingMode() const;
// [ 4] bool skipUnknownElements() const;
// [ 4] bool validateInputIsUtf8() const;
// [ 4] bool validateRootTag() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const balxml::DecoderOptions& lhs, rhs);
// [ 6] bool operator!=(const balxml::DecoderOptions& lhs, rhs);
// [ 5] operator<<(ostream& s, const balxml::DecoderOptions& d);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balxml::DecoderOptions Obj;

typedef bdlat_FormattingMode   Mode;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA (potentially) used by test cases 3, 7, (8), 9, and (10)

struct DefaultDataRow {
    int         d_line;           // source line number
    int         d_maxDepth;
    int         d_formattingMode;
    bool        d_skipUnknownElements;
    bool        d_validateInputIsUtf8;
    bool        d_validateRootTag;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE DEPTH     MODE    SKIP   UTF8   ROOT
    //---- -------   ----    ----   ----   ----

    { L_,      512,
          Mode::e_DEFAULT,  true, false, false } // default first

    // DEPTH: INT_MIN
,   { L_,  INT_MIN,
          Mode::e_DEFAULT, false, false, false }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT, false, false,  true }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT, false,  true, false }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT, false,  true,  true }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT,  true, false, false }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT,  true, false,  true }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT,  true,  true, false }
,   { L_,  INT_MIN,
          Mode::e_DEFAULT,  true,  true,  true }

,   { L_,  INT_MIN,
                  INT_MIN, false, false, false }
,   { L_,  INT_MIN,
                  INT_MIN, false, false,  true }
,   { L_,  INT_MIN,
                  INT_MIN, false,  true, false }
,   { L_,  INT_MIN,
                  INT_MIN, false,  true,  true }
,   { L_,  INT_MIN,
                  INT_MIN,  true, false, false }
,   { L_,  INT_MIN,
                  INT_MIN,  true, false,  true }
,   { L_,  INT_MIN,
                  INT_MIN,  true,  true, false }
,   { L_,  INT_MIN,
                  INT_MIN,  true,  true,  true }

,   { L_,  INT_MIN,
                  INT_MAX, false, false, false }
,   { L_,  INT_MIN,
                  INT_MAX, false, false,  true }
,   { L_,  INT_MIN,
                  INT_MAX, false,  true, false }
,   { L_,  INT_MIN,
                  INT_MAX, false,  true,  true }
,   { L_,  INT_MIN,
                  INT_MAX,  true, false, false }
,   { L_,  INT_MIN,
                  INT_MAX,  true, false,  true }
,   { L_,  INT_MIN,
                  INT_MAX,  true,  true, false }
,   { L_,  INT_MIN,
                  INT_MAX,  true,  true,  true }

    // DEPTH: INT_MAX
,   { L_,  INT_MAX,
          Mode::e_DEFAULT, false, false, false }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT, false, false,  true }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT, false,  true, false }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT, false,  true,  true }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT,  true, false, false }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT,  true, false,  true }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT,  true,  true, false }
,   { L_,  INT_MAX,
          Mode::e_DEFAULT,  true,  true,  true }

,   { L_,  INT_MAX,
                  INT_MIN, false, false, false }
,   { L_,  INT_MAX,
                  INT_MIN, false, false,  true }
,   { L_,  INT_MAX,
                  INT_MIN, false,  true, false }
,   { L_,  INT_MAX,
                  INT_MIN, false,  true,  true }
,   { L_,  INT_MAX,
                  INT_MIN,  true, false, false }
,   { L_,  INT_MAX,
                  INT_MIN,  true, false,  true }
,   { L_,  INT_MAX,
                  INT_MIN,  true,  true, false }
,   { L_,  INT_MAX,
                  INT_MIN,  true,  true,  true }

,   { L_,  INT_MAX,
                  INT_MAX, false, false, false }
,   { L_,  INT_MAX,
                  INT_MAX, false, false,  true }
,   { L_,  INT_MAX,
                  INT_MAX, false,  true, false }
,   { L_,  INT_MAX,
                  INT_MAX, false,  true,  true }
,   { L_,  INT_MAX,
                  INT_MAX,  true, false, false }
,   { L_,  INT_MAX,
                  INT_MAX,  true, false,  true }
,   { L_,  INT_MAX,
                  INT_MAX,  true,  true, false }
,   { L_,  INT_MAX,
                  INT_MAX,  true,  true,  true }

};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and populating an options object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for decoding objects in the JSON format.  This example shows how to create
// and populate an options object.
//
// First, we default-construct a `balxml::DecoderOptions` object:
// ```
    const int  MAX_DEPTH             = 10;
    const bool SKIP_UNKNOWN_ELEMENTS = false;

    balxml::DecoderOptions options;
    ASSERT(512   == options.maxDepth());
    ASSERT(true == options.skipUnknownElements());
// ```
// Next, we populate that object to decode using a different `maxDepth` value
// and `skipUnknownElements` value:
// ```
    options.setMaxDepth(MAX_DEPTH);
    ASSERT(MAX_DEPTH == options.maxDepth());

    options.setSkipUnknownElements(SKIP_UNKNOWN_ELEMENTS);
    ASSERT(SKIP_UNKNOWN_ELEMENTS == options.skipUnknownElements());
// ```
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for `swap` testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        // 1. The assignment operator can change the value of any modifiable
        //    target object to that of any source object.
        //
        // 2. The signature and return type are standard.
        //
        // 3. The reference returned is to the target object (i.e., `*this`).
        //
        // 4. The value of the source object is not modified.
        //
        // 5. Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        // 1. Use the address of `operator=` to initialize a member-function
        //    pointer having the appropriate signature and return type for the
        //    copy-assignment operator defined in this component.  (C-2)
        //
        // 2. Using the table-driven technique, specify a set of (unique) valid
        //    object values (one per row) in terms of their individual
        //    attributes, including (a) first, the default value, and (b)
        //    boundary values corresponding to every range of values that each
        //    individual attribute can independently attain.
        //
        // 3. For each row `R1` (representing a distinct object value, `V`) in
        //    the table described in P-3:  (C-1, 3..4)
        //
        //   1. Create two `const` `Obj`, `Z` and `ZZ`, each having the value
        //      `V`.
        //
        //   2. Execute an inner loop that iterates over each row `R2`
        //      (representing a distinct object value, `W`) in the table
        //      described in P-3:
        //
        //   3. For each of the iterations (P-3.2):  (C-1, 3..4)
        //
        //     1. Use the value constructor to create a modifiable `Obj`, `mX`,
        //        having the value `W`.
        //
        //     2. Assign `mX` from `Z`.
        //
        //     3. Verify that the address of the return value is the same as
        //        that of `mX`.  (C-3)
        //
        //     4. Use the equality-comparison operator to verify that: (C-1, 4)
        //
        //       1. The target object, `mX`, now has the same value as that of
        //          `Z`.  (C-1)
        //
        //       2. `Z` still has the same value as that of `ZZ`.  (C-4)
        //
        // 4. Repeat steps similar to those described in P-2 except that, this
        //    time, the source object, `Z`, is a reference to the target
        //    object, `mX`, and both `mX` and `ZZ` are initialized to have the
        //    value `V`.  For each row (representing a distinct object value,
        //    `V`) in the table described in P-2:  (C-5)
        //
        //   1. Use the value constructor to create a modifiable `Obj` `mX`;
        //      also use the value constructor to create a `const` `Obj` `ZZ`.
        //
        //   2. Let `Z` be a reference providing only `const` access to `mX`.
        //
        //   3. Assign `mX` from `Z`.
        //
        //   4. Verify that the address of the return value is the same as that
        //      of `mX`.  (C-3)
        //
        //   5. Use the equality-comparison operator to verify that the
        //      target object, `Z`, still has the same value as that of `ZZ`.
        //      (C-5)
        //
        // Testing:
        //   operator=(const balxml::DecoderOptions& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY-ASSIGNMENT OPERATOR" << endl
                          << "================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << "\nUse table of distinct object values." << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
                         "\nCopy-assign every value into every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1  = DATA[ti].d_line;
            const int   DEPTH1 = DATA[ti].d_maxDepth;
            const int   MODE1  = DATA[ti].d_formattingMode;
            const bool  SKIP1  = DATA[ti].d_skipUnknownElements;
            const bool  UTF81  = DATA[ti].d_validateInputIsUtf8;
            const bool  ROOT1  = DATA[ti].d_validateRootTag;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setMaxDepth(DEPTH1);
            mZ.setFormattingMode(MODE1);
            mZ.setSkipUnknownElements(SKIP1);
            mZ.setValidateInputIsUtf8(UTF81);
            mZ.setValidateRootTag(ROOT1);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setMaxDepth(DEPTH1);
            mZZ.setFormattingMode(MODE1);
            mZZ.setSkipUnknownElements(SKIP1);
            mZZ.setValidateInputIsUtf8(UTF81);
            mZZ.setValidateRootTag(ROOT1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }
            else {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() != Z);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2  = DATA[tj].d_line;
                const int   DEPTH2 = DATA[tj].d_maxDepth;
                const int   MODE2  = DATA[tj].d_formattingMode;
                const bool  SKIP2  = DATA[tj].d_skipUnknownElements;
                const bool  UTF82  = DATA[tj].d_validateInputIsUtf8;
                const bool  ROOT2  = DATA[tj].d_validateRootTag;

                Obj mX;  const Obj& X = mX;
                mX.setMaxDepth(DEPTH2);
                mX.setFormattingMode(MODE2);
                mX.setSkipUnknownElements(SKIP2);
                mX.setValidateInputIsUtf8(UTF82);
                mX.setValidateRootTag(ROOT2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                LOOP4_ASSERT(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            if (verbose) cout << "Testing self-assignment" << endl;

            {
                Obj mX;
                mX.setMaxDepth(DEPTH1);
                mX.setSkipUnknownElements(SKIP1);
                mX.setValidateInputIsUtf8(UTF81);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setMaxDepth(DEPTH1);
                mZZ.setSkipUnknownElements(SKIP1);
                mZZ.setValidateInputIsUtf8(UTF81);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);
                LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for `swap` testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "======================================" << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        // 1. The copy constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The original object is passed as a reference providing
        //    non-modifiable access to that object.
        //
        // 3. The value of the original object is unchanged.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of (unique) valid
        //    object values (one per row) in terms of their individual
        //    attributes, including (a) first, the default value, and (b)
        //    boundary values corresponding to every range of values that each
        //    individual attribute can independently attain.
        //
        // 2. For each row (representing a distinct object value, `V`) in the
        //    table described in P-1:  (C-1..3)
        //
        //   1. Use the value constructor to create two `const` `Obj`, `Z` and
        //      `ZZ`, each having the value `V`.
        //
        //   2. Use the copy constructor to create an object `X`,
        //      supplying it the `const` object `Z`.  (C-2)
        //
        //   3. Use the equality-comparison operator to verify that:
        //      (C-1, 3)
        //
        //     1. The newly constructed object, `X`, has the same value as `Z`.
        //        (C-1)
        //
        //     2. `Z` still has the same value as `ZZ`.  (C-3)
        //
        // Testing:
        //   balxml::DecoderOptions(const balxml::DecoderOptions& o);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nUse table of distinct object values." << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
                        "\nCopy construct an object from every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_line;
            const int   DEPTH = DATA[ti].d_maxDepth;
            const int   MODE  = DATA[ti].d_formattingMode;
            const bool  SKIP  = DATA[ti].d_skipUnknownElements;
            const bool  UTF8  = DATA[ti].d_validateInputIsUtf8;
            const bool  ROOT  = DATA[ti].d_validateRootTag;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setMaxDepth(DEPTH);
            mZ.setFormattingMode(MODE);
            mZ.setSkipUnknownElements(SKIP);
            mZ.setValidateInputIsUtf8(UTF8);
            mZ.setValidateRootTag(ROOT);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setMaxDepth(DEPTH);
            mZZ.setFormattingMode(MODE);
            mZZ.setSkipUnknownElements(SKIP);
            mZZ.setValidateInputIsUtf8(UTF8);
            mZZ.setValidateRootTag(ROOT);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), Z, Obj() == Z)
                firstFlag = false;
            }
            else {
                LOOP3_ASSERT(LINE, Obj(), Z, Obj() != Z)
            }

            // Verify the value of the object.

            LOOP3_ASSERT(LINE,  Z, X,  Z == X);

            // Verify that the value of `Z` has not changed.

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }  // end foreach row
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //   Ensure that `==` and `!=` are the operational definition of value.
        //
        // Concerns:
        // 1. Two objects, `X` and `Y`, compare equal if and only if each of
        //    their corresponding salient attributes respectively compares
        //    equal.
        //
        // 2. All salient attributes participate in the comparison.
        //
        // 3. No non-salient attributes (i.e., `allocator`) participate.
        //
        // 4. `true  == (X == X)`  (i.e., identity)
        //
        // 5. `false == (X != X)`  (i.e., identity)
        //
        // 6. `X == Y` if and only if `Y == X`  (i.e., commutative)
        //
        // 7. `X != Y` if and only if `Y != X`  (i.e., commutative)
        //
        // 8. `X != Y` if and only if `!(X == Y)`
        //
        // 9. Comparison is symmetric with respect to user-defined conversion
        //    (i.e., both comparison operators are free functions).
        //
        // 10. Non-modifiable objects can be compared (i.e., objects or
        //    references providing only non-modifiable access).
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their individual salient
        //    attributes such that for each salient attribute, there exists a
        //    pair of rows that differ (slightly) in only the column
        //    corresponding to that attribute.
        //
        // 2. For each row `R1` in the table of P-3:  (C-1..8)
        //
        //   1. Create a single object, and use it to verify the reflexive
        //      (anti-reflexive) property of equality (inequality) in the
        //      presence of aliasing.  (C-4..5)
        //
        //   2. For each row `R2` in the table of P-3:  (C-1..3, 6..8)
        //
        //     1. Record, in `EXP`, whether or not distinct objects created
        //        from `R1` and `R2`, respectively, are expected to have the
        //        same value.
        //
        //     2. Create an object `X` having the value `R1`.
        //
        //     3. Create an object `Y` having the value `R2`.
        //
        //     4. Verify the commutative property and expected return value for
        //        both `==` and `!=`.  (C-1..3, 6..8)
        //
        // Testing:
        //   bool operator==(const balxml::DecoderOptions& lhs, rhs);
        //   bool operator!=(const balxml::DecoderOptions& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, `Ai` and `Bi`."
                                                                       << endl;
        // ---------------
        // Attribute Types
        // ---------------

        typedef int   T1;        // `maxDepth`
        typedef int   T2;        // `formattingMode`
        typedef bool  T3;        // `skipUnknownElements`
        typedef bool  T4;        // `validateInputIsUtf8`
        typedef bool  T5;        // `validateRootTag`

                      // ------------------------------
                      // Attribute 1 Values: `maxDepth`
                      // ------------------------------

        const T1 A1 = 512;                   // baseline
        const T1 B1 = 0;

                    // ------------------------------------
                    // Attribute 2 Values: `formattingMode`
                    // ------------------------------------

        const T2 A2 = 0;                     // baseline
        const T2 B2 = Mode::e_DEC;

                // -----------------------------------------
                // Attribute 3 Values: `skipUnknownElements`
                // -----------------------------------------

        const T3 A3 = true;                  // baseline
        const T3 B3 = false;

                // -----------------------------------------
                // Attribute 4 Values: `validateInputIsUtf8`
                // -----------------------------------------

        const T4 A4 = false;                 // baseline
        const T4 B4 = true;

                   // -------------------------------------
                   // Attribute 5 Values: `validateRootTag`
                   // -------------------------------------

        const T5 A5 = false;                 // baseline
        const T5 B5 = true; 

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int   d_line;        // source line number
            int   d_maxDepth;
            int   d_formattingMode;
            bool  d_skipUnknownElements;
            bool  d_validateInputIsUtf8;
            bool  d_validateRootTag;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute value
        // (Bi).

            //LINE DEPTH     MODE   SKIP   UTF8   ROOT
            //---- -------   ----   ----   ----   ----

            { L_,      A1,   A2,    A3,     A4,     A5 }, // baseline

            { L_,      B1,   A2,    A3,     A4,     A5 },
            { L_,      A1,   B2,    A3,     A4,     A5 },
            { L_,      A1,   A2,    B3,     A4,     A5 },
            { L_,      A1,   A2,    A3,     B4,     A5 },
            { L_,      A1,   A2,    A3,     A4,     B5 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1  = DATA[ti].d_line;
            const int   DEPTH1 = DATA[ti].d_maxDepth;
            const int   MODE1  = DATA[ti].d_formattingMode;
            const bool  SKIP1  = DATA[ti].d_skipUnknownElements;
            const bool  UTF81  = DATA[ti].d_validateInputIsUtf8;
            const bool  ROOT1  = DATA[ti].d_validateRootTag;

            if (veryVerbose) {
                T_ P_(LINE1) P_(DEPTH1)
                             P_(MODE1)
                             P_(SKIP1)
                             P_(UTF81)
                             P(ROOT1)
            }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mX;  const Obj& X = mX;

                mX.setMaxDepth(DEPTH1);
                mX.setFormattingMode(MODE1);
                mX.setSkipUnknownElements(SKIP1);
                mX.setValidateInputIsUtf8(UTF81);
                mX.setValidateRootTag(ROOT1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2  = DATA[tj].d_line;
                const int   DEPTH2 = DATA[tj].d_maxDepth;
                const int   MODE2  = DATA[tj].d_formattingMode;
                const bool  SKIP2  = DATA[tj].d_skipUnknownElements;
                const bool  UTF82  = DATA[tj].d_validateInputIsUtf8;
                const bool  ROOT2  = DATA[tj].d_validateRootTag;

                const bool EXP = ti == tj;  // expected for equality comparison

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(DEPTH2)
                                    P_(MODE2)
                                    P_(SKIP2)
                                    P_(UTF82)
                                    P(ROOT2)
                }

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setMaxDepth(DEPTH1);
                mX.setFormattingMode(MODE1);
                mX.setSkipUnknownElements(SKIP1);
                mX.setValidateInputIsUtf8(UTF81);
                mX.setValidateRootTag(ROOT1);

                mY.setMaxDepth(DEPTH2);
                mY.setFormattingMode(MODE2);
                mY.setSkipUnknownElements(SKIP2);
                mY.setValidateInputIsUtf8(UTF82);
                mY.setValidateRootTag(ROOT2);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify value, and commutative.

                LOOP4_ASSERT(LINE1, LINE2, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an `ostream` in some standard, human-readable form.
        //
        // Concerns:
        // 1. The `print` method writes the value to the specified `ostream`.
        //
        // 2. The `print` method writes the value in the intended format.
        //
        // 3. The output using `s << obj` is the same as `obj.print(s, 0, -1)`,
        //    but with each "attributeName = " elided.
        //
        // 4. The output `operator<<` returns the supplied `ostream`.
        //
        // Plan:
        // 1. Using the table-driven technique, define twelve carefully
        //    selected combinations of (two) object values (`A` and `B`),
        //    having distinct values for each corresponding salient attribute,
        //    and various values for the two formatting parameters, along with
        //    the expected output
        //      ( `value` x  `level`   x `skipUnknownElements` ):
        //     1. { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //     2. { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //     3. { B   } x {  2     } x {  3        }  -->  1 expected output
        //     4. { A B } x { -9     } x { -9        }  -->  2 expected outputs
        //
        // 2. For each row in the table defined in P-2.1:  (C-1..5)
        //
        //   1. Using a `const` `Obj`, supply each object value and pair of
        //      formatting parameters to `print`, unless the parameters are,
        //      arbitrarily, (-9, -9), in which case `operator<<` will be
        //      invoked instead.
        //
        //   2. Use a standard `ostringstream` to capture the actual output.
        //
        //   3. Verify the address of what is returned is that of the
        //      supplied stream.  (C-4, 5)
        //
        //   4. Compare the contents captured in P-2.2.2 with what is
        //      expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const balxml::DecoderOptions& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OUTPUT OPERATOR" << endl
                          << "=================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spl;

            int         d_maxDepth;
            int         d_formattingMode;
            bool        d_skipUnknownElements;
            bool        d_validateInputIsUtf8;
            bool        d_validateRootTag;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

#define D  Mode::e_DEC
#define H  Mode::e_HEX

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1 } --> 3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  MD  F  S  U  R  EXP
        //---- - ---  --- -  -  -  -  ---

        { L_,  0,  0, 89, D, 0, 1, 1,
                                 "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "formattingMode = 1"                        NL
                                 "skipUnknownElements = false"               NL
                                 "validateInputIsUtf8 = true"                NL
                                 "validateRootTag = true"                    NL
                                 "]"                                         NL
                                                                             },

        { L_,  0,  1, 89, D, 0, 1, 1,
                                 "["                                         NL
                                 " maxDepth = 89"                            NL
                                 " formattingMode = 1"                       NL
                                 " skipUnknownElements = false"              NL
                                 " validateInputIsUtf8 = true"               NL
                                 " validateRootTag = true"                   NL
                                 "]"                                         NL
                                                                             },

        { L_,  0, -1, 89, H, 1, 0, 0,
                                 "["                                         SP
                                 "maxDepth = 89"                             SP
                                 "formattingMode = 2"                        SP
                                 "skipUnknownElements = true"                SP
                                 "validateInputIsUtf8 = false"               SP
                                 "validateRootTag = false"                   SP
                                 "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 } --> 6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  MD  F  S  U  R  EXP
        //---- - ---  --- -  -  -  -  ---

        { L_,  3,  0, 89, D, 1, 1, 1,
                                 "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "formattingMode = 1"                        NL
                                 "skipUnknownElements = true"                NL
                                 "validateInputIsUtf8 = true"                NL
                                 "validateRootTag = true"                    NL
                                 "]"                                         NL
                                                                             },

        { L_,  3,  2, 89, D, 0, 0, 0,
                         "      ["                                           NL
                         "        maxDepth = 89"                             NL
                         "        formattingMode = 1"                        NL
                         "        skipUnknownElements = false"               NL
                         "        validateInputIsUtf8 = false"               NL
                         "        validateRootTag = false"                   NL
                         "      ]"                                           NL
                                                                             },

        { L_,  3, -2, 89, H, 1, 0, 0,
                                 "      ["                                   SP
                                 "maxDepth = 89"                             SP
                                 "formattingMode = 2"                        SP
                                 "skipUnknownElements = true"                SP
                                 "validateInputIsUtf8 = false"               SP
                                 "validateRootTag = false"                   SP
                                 "]"
                                                                             },

        { L_, -3,  0, 89, D, 0, 1, 1,
                                 "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "formattingMode = 1"                        NL
                                 "skipUnknownElements = false"               NL
                                 "validateInputIsUtf8 = true"                NL
                                 "validateRootTag = true"                    NL
                                 "]"                                         NL
                                                                             },

        { L_, -3,  2, 89, D, 0, 0, 0,
                        "["                                                  NL
                        "        maxDepth = 89"                              NL
                        "        formattingMode = 1"                         NL
                        "        skipUnknownElements = false"                NL
                        "        validateInputIsUtf8 = false"                NL
                        "        validateRootTag = false"                    NL
                        "      ]"                                            NL
                                                                             },

        { L_, -3, -2, 89, H, 1, 1, 1,
                                 "["                                         SP
                                 "maxDepth = 89"                             SP
                                 "formattingMode = 2"                        SP
                                 "skipUnknownElements = true"                SP
                                 "validateInputIsUtf8 = true"                SP
                                 "validateRootTag = true"                    SP
                                 "]"
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  MD  F  S  U  R  EXP
        //---- - ---  --- -  -  -  -  ---

        { L_,  2,  3, 89, D, 0, 1, 1,
                        "      ["                                            NL
                        "         maxDepth = 89"                             NL
                        "         formattingMode = 1"                        NL
                        "         skipUnknownElements = false"               NL
                        "         validateInputIsUtf8 = true"                NL
                        "         validateRootTag = true"                    NL
                        "      ]"                                            NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 } x { -9 } --> 2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  MD  F  S  U  R  EXP
        //---- - ---  --- -  -  -  -  ---

        { L_, -9, -9, 89, D, 1, 0, 1,
                                 "["                                         SP
                                 "maxDepth = 89"                             SP
                                 "formattingMode = 1"                        SP
                                 "skipUnknownElements = true"                SP
                                 "validateInputIsUtf8 = false"               SP
                                 "validateRootTag = true"                    SP
                                 "]"                                         },

        { L_, -9, -9,  7, H, 1, 1, 0,
                                 "["                                         SP
                                 "maxDepth = 7"                              SP
                                 "formattingMode = 2"                        SP
                                 "skipUnknownElements = true"                SP
                                 "validateInputIsUtf8 = true"                SP
                                 "validateRootTag = false"                   SP
                                 "]"                                         },

#undef NL
#undef SP

#undef D
#undef H

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const int   L          = DATA[ti].d_level;
                const int   S          = DATA[ti].d_spl;

                const int   MAX_DEPTH  = DATA[ti].d_maxDepth;
                const int   MODE       = DATA[ti].d_formattingMode;
                const int   SKIP_ELEMS = DATA[ti].d_skipUnknownElements;
                const int   VALID_UTF8 = DATA[ti].d_validateInputIsUtf8;
                const int   VALID_ROOT = DATA[ti].d_validateRootTag;

                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(MAX_DEPTH) P_(SKIP_ELEMS)}

                if (veryVerbose) {
                    T_ P_(L) P_(MAX_DEPTH)
                             P_(MODE)
                             P_(SKIP_ELEMS)
                             P_(VALID_UTF8)
                             P(VALID_ROOT)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                mX.setMaxDepth(MAX_DEPTH);
                mX.setFormattingMode(MODE);
                mX.setSkipUnknownElements(SKIP_ELEMS);
                mX.setValidateInputIsUtf8(VALID_UTF8);
                mX.setValidateRootTag(VALID_ROOT);

                ostringstream os;

                if (-9 == L && -9 == S) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, S));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        // 1. Each accessor returns the value of the corresponding attribute
        //    of the object.
        //
        // 2. Each accessor method is declared `const`.
        //
        // Plan:
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        // 1. Use the default constructor to create an object (having default
        //    attribute values).
        //
        // 2. Verify that each basic accessor, invoked on a reference providing
        //    non-modifiable access to the object created in P2, returns the
        //    expected value.  (C-2)
        //
        // 3. For each salient attribute (contributing to value):  (C-1)
        //   1. Use the corresponding primary manipulator to set the attribute
        //      to a unique value.
        //
        //   2. Use the corresponding basic accessor to verify the new
        //      expected value.  (C-1)
        //
        // Testing:
        //   int  maxDepth() const;
        //   int  formattingMode() const;
        //   bool skipUnknownElements() const;
        //   bool validateInputIsUtf8() const;
        //   bool validateRootTag() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        // Attribute Types

        typedef int   T1;        // `maxDepth`
        typedef int   T2;        // `formattingMode`
        typedef bool  T3;        // `skipUnknownElements`
        typedef bool  T4;        // `validateInputIsUtf8`
        typedef bool  T5;        // `validateRootTag`

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

          // -----------------------------------------------------
          // `D` values: These are the default-constructed values.
          // -----------------------------------------------------

        const T1   D1 = 512;              // `maxDepth`
        const T2   D2 = Mode::e_DEFAULT;  // `formattingMode`
        const T3   D3 = true;             // `skipUnknownElements`
        const T4   D4 = false;            // `validateInputIsUtf8`
        const T5   D5 = false;            // `validateRootTag`

                    // ----------------------------
                    // `A` values: Boundary values.
                    // ----------------------------

        const T1    A1   = INT_MAX;        // `maxDepth`
        const T2    A2   = Mode::e_TEXT;   // `formattingMode`
        const T3    A3   = false;          // `skipUnknownElements`
        const T4    A4   = true;           // `validateInputIsUtf8`
        const T5    A5   = true;           // `validateRootTag`

        if (verbose) cout << "\nCreate an object." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1 maxDepth =
                   X.maxDepth();
            ASSERTV(D1,   maxDepth,
                    D1 == maxDepth);

            const T2 formattingMode =
                   X.formattingMode();
            ASSERTV(D2,   formattingMode,
                    D2 == formattingMode);

            const T3 skipUnknownElements =
                   X.skipUnknownElements();
            ASSERTV(D3,   skipUnknownElements,
                    D3 == skipUnknownElements);

            const T4 validateInputIsUtf8 =
                   X.validateInputIsUtf8();
            ASSERTV(D4,   validateInputIsUtf8,
                    D4 == validateInputIsUtf8);

            const T5 validateRootTag =
                   X.validateRootTag();
            ASSERTV(D5,   validateRootTag,
                    D5 == validateRootTag);
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(maxDepth) }
        {
            mX.setMaxDepth(A1);

            const T1 maxDepth = X.maxDepth();
            ASSERTV(A1,   maxDepth,
                    A1 == maxDepth);
        }

        if (veryVerbose) { T_ Q(formattingMode) }
        {
            mX.setFormattingMode(A2);

            const T2 formattingMode =
                   X.formattingMode();
            ASSERTV(A2,   formattingMode,
                    A2 == formattingMode);
        }

        if (veryVerbose) { T_ Q(skipUnknownElements) }
        {
            mX.setSkipUnknownElements(A3);

            const T3 skipUnknownElements =
                   X.skipUnknownElements();
            ASSERTV(A3,   skipUnknownElements,
                    A3 == skipUnknownElements);
        }

        if (veryVerbose) { T_ Q(validateInputIsUtf8) }
        {
            mX.setValidateInputIsUtf8(A4);

            const T4 validateInputIsUtf8 =
                   X.validateInputIsUtf8();
            ASSERTV(A4,   validateInputIsUtf8,
                    A4 == validateInputIsUtf8);
        }

        if (veryVerbose) { T_ Q(validateRootTag) }
        {
            mX.setValidateRootTag(A5);

            const T5 validateRootTag =
                   X.validateRootTag();
            ASSERTV(A5,   validateRootTag,
                    A5 == validateRootTag);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        // 1. Any argument can be `const`.
        //
        // 2. Each attribute is modifiable independently.
        //
        // 3. Each attribute can be set to represent any value that does not
        //    violate that attribute's documented constraints.
        //
        // 4. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create three sets of attribute values for the object: `D`, `A`,
        //    and `B`.  `D` values correspond to the attribute values, and `A`
        //    and `B` values are chosen to be distinct boundary values where
        //    possible.
        //
        // 2. Use the default constructor to create an object `X`.
        //
        // 3. For each attribute `i`, in turn, create a local block.  Then
        //    inside the block, using brute force, set that attribute's
        //    value, passing a `const` argument representing each of the
        //    three test values, in turn (see P-1), first to `Ai`, then to
        //    `Bi`, and finally back to `Di`.  After each transition, use the
        //    (as yet unproven) basic accessors to verify that only the
        //    intended attribute value changed.  (C-1, 3)
        //
        // 4. Corroborate that attributes are modifiable independently by
        //    first setting all of the attributes to their `A` values.  Then
        //    incrementally set each attribute to its `B` value and verify
        //    after each manipulation that only that attribute's value
        //    changed.  (C-2)
        //
        // 5. Verify that, in appropriate build modes, defensive checks are
        //    triggered for invalid attribute values, but not triggered for
        //    adjacent valid ones (using the `BSLS_ASSERTTEST_*` macros).
        //    (C-4)
        //
        // Testing:
        //   setMaxDepth(int value);
        //   setFormattingMode(int value);
        //   setSkipUnknownElements(int value);
        //   setValidateInputIsUtf8(bool value);
        //   setValidateRootTag(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS" << endl
                          << "====================" << endl;

        // Attribute Types

        typedef int   T1;        // `maxDepth`
        typedef int   T2;        // `formattingMode`
        typedef bool  T3;        // `skipUnknownElements`
        typedef bool  T4;        // `validateInputIsUtf8`
        typedef bool  T5;        // `validateRootTag`

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // `D` values: These are the default-constructed values.

        const T1   D1 = 512;              // `maxDepth`
        const T2   D2 = Mode::e_DEFAULT;  // `formattingMode`
        const T3   D3 = true;             // `skipUnknownElements`
        const T4   D4 = false;            // `validateInputIsUtf8`
        const T5   D5 = false;            // `validateRootTag`

        // `A` values.

        const T1    A1   = INT_MAX;        // `maxDepth`
        const T2    A2   = Mode::e_TEXT;   // `formattingMode`
        const T3    A3   = false;          // `skipUnknownElements`
        const T4    A4   = true;           // `validateInputIsUtf8`
        const T5    A5   = true;           // `validateRootTag`

        // `B` values.

        const T1    B1   = 0;        // `maxDepth`
        const T2    B2   = Mode::e_BASE64; // `formattingMode`
        const T3    B3   = true;           // `skipUnknownElements`
        const T4    B4   = false;          // `validateInputIsUtf8`
        const T5    B5   = false;          // `validateRootTag`

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // ----------
        // `maxDepth`
        // ----------
        {
            mX.setMaxDepth(A1);
            ASSERT(A1 == X.maxDepth());                         // <= 1
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setMaxDepth(B1);
            ASSERT(B1 == X.maxDepth());                         // <= 1
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setMaxDepth(D1);
            ASSERT(D1 == X.maxDepth());                         // <= 1
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());
        }

        // ----------------
        // `formattingMode`
        // ----------------
        {
            mX.setFormattingMode(A2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(A2 == X.formattingMode());                  // <= 2
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setFormattingMode(B2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(B2 == X.formattingMode());                  // <= 2
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setFormattingMode(D2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());                  // <= 2
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());
        }

        // ---------------------
        // `skipUnknownElements`
        // ---------------------
        {
            mX.setSkipUnknownElements(A3);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(A3 == X.skipUnknownElements());              // <= 3
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setSkipUnknownElements(B3);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(B3 == X.skipUnknownElements());              // <= 3
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());

            mX.setSkipUnknownElements(D3);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());              // <= 3
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());
        }

        // ---------------------
        // `validateInputIsUtf8`
        // ---------------------
        {
            mX.setValidateInputIsUtf8(A4);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(A4 == X.validateInputIsUtf8());              // <= 4
            ASSERT(D5 == X.validateRootTag());

            mX.setValidateInputIsUtf8(B4);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(B4 == X.validateInputIsUtf8());              // <= 4
            ASSERT(D5 == X.validateRootTag());

            mX.setValidateInputIsUtf8(D4);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());              // <= 4
            ASSERT(D5 == X.validateRootTag());
        }

        // -----------------
        // `validateRootTag`
        // -----------------
        {
            mX.setValidateRootTag(A5);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(A5 == X.validateRootTag());                  // <= 5

            mX.setValidateRootTag(B5);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(B5 == X.validateRootTag());                  // <= 5

            mX.setValidateRootTag(D5);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.formattingMode());
            ASSERT(D3 == X.skipUnknownElements());
            ASSERT(D4 == X.validateInputIsUtf8());
            ASSERT(D5 == X.validateRootTag());                  // <= 5
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their `A` values.
            // ---------------------------------------

            mX.setMaxDepth(A1);
            mX.setSkipUnknownElements(A3);
            mX.setValidateInputIsUtf8(A4);

            ASSERT(A1 == X.maxDepth());
            ASSERT(A3 == X.skipUnknownElements());
            ASSERT(A4 == X.validateInputIsUtf8());

                 // ---------------------------------------
                 // Set all attributes to their `B` values.
                 // ---------------------------------------

            mX.setMaxDepth(B1);

            ASSERT(B1 == X.maxDepth());
            ASSERT(A3 == X.skipUnknownElements());
            ASSERT(A4 == X.validateInputIsUtf8());

            mX.setSkipUnknownElements(B3);

            ASSERT(B1 == X.maxDepth());
            ASSERT(B3 == X.skipUnknownElements());
            ASSERT(A4 == X.validateInputIsUtf8());

            mX.setValidateInputIsUtf8(B4);

            ASSERT(B1 == X.maxDepth());
            ASSERT(B3 == X.skipUnknownElements());
            ASSERT(B4 == X.validateInputIsUtf8());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        // 1. An object created with the default constructor has the
        //    contractually specified default value.
        //
        // Plan:
        // 1. Use the default constructor to create an object `X`.
        //
        // 2. Use the individual (as yet unproven) salient attribute
        //    accessors to verify the default-constructed value.  (C-1)
        //
        // Testing:
        //   balxml::DecoderOptions();
        //   ~balxml::DecoderOptions();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // `D` values: These are the default-constructed values.

        const int   D1   = 512;    // `maxDepth`
        const int   D2   = 0;      // `formattingMode`
        const bool  D3   = true;   // `skipUnknownElements`
        const bool  D4   = false;  // `validateInputIsUtf8`
        const bool  D5   = false;  // `validateRootTag`
        if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout << "Verify the object's attribute values." << endl;

                  // -------------------------------------
                  // Verify the object's attribute values.
                  // -------------------------------------

        LOOP2_ASSERT(D1,   X.maxDepth(),
                     D1 == X.maxDepth());
        LOOP2_ASSERT(D2,   X.formattingMode(),
                     D2 == X.formattingMode());
        LOOP2_ASSERT(D3,   X.skipUnknownElements(),
                     D3 == X.skipUnknownElements());
        LOOP2_ASSERT(D4,   X.validateInputIsUtf8(),
                     D4 == X.validateInputIsUtf8());
        LOOP2_ASSERT(D5,   X.validateRootTag(),
                     D5 == X.validateRootTag());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create an object `w` (default ctor).       { w:D             }
        // 2. Create an object `x` (copy from `w`).      { w:D x:D         }
        // 3. Set `x` to `A` (value distinct from `D`).  { w:D x:A         }
        // 4. Create an object `y` (init. to `A`).       { w:D x:A y:A     }
        // 5. Create an object `z` (copy from `y`).      { w:D x:A y:A z:A }
        // 6. Set `z` to `D` (the default value).        { w:D x:A y:A z:D }
        // 7. Assign `w` from `x`.                       { w:A x:A y:A z:D }
        // 8. Assign `w` from `z`.                       { w:D x:A y:A z:D }
        // 9. Assign `x` from `x` (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute Types

        typedef int   T1;        // `maxDepth`
        typedef int   T2;        // `formattingMode`
        typedef bool  T3;        // `skipUnknownElements`
        typedef bool  T4;        // `validateInputIsUtf8`
        typedef bool  T5;        // `validateRootTag`

        // Attribute 1 Values: `maxDepth`

        const T1 D1 = 512;     // default value
        const T1 A1 = 0;

        // Attribute 2 Values: `formattingMode`

        const T2 D2 = 0;       // default value
        const T2 A2 = Mode::e_DEC;

        // Attribute 3 Values: `skipUnknownElements`

        const T3 D3 = true;     // default value
        const T3 A3 = false;

        // Attribute 4 Values: `validateInputIsUtf8`

        const T4 D4 = false;    // default value
        const T4 A4 = true;

        // Attribute 5 Values: `validateRootTag`

        const T5 D5 = false;    // default value
        const T5 A5 = true;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object `w` (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of `w`." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.maxDepth());
        ASSERT(D2 == W.formattingMode());
        ASSERT(D3 == W.skipUnknownElements());
        ASSERT(D4 == W.validateInputIsUtf8());
        ASSERT(D5 == W.validateRootTag());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: `w` <op> `w`." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object `x` (copy from `w`)."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of `x`." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.maxDepth());
        ASSERT(D2 == X.formattingMode());
        ASSERT(D3 == X.skipUnknownElements());
        ASSERT(D4 == X.validateInputIsUtf8());
        ASSERT(D5 == X.validateRootTag());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: `x` <op> `w`, `x`." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set `x` to `A` (value distinct from `D`)."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setMaxDepth(A1);
        mX.setFormattingMode(A2);
        mX.setSkipUnknownElements(A3);
        mX.setValidateInputIsUtf8(A4);
        mX.setValidateRootTag(A4);

        if (veryVerbose) cout << "\ta. Check new value of `x`." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.maxDepth());
        ASSERT(A2 == X.formattingMode());
        ASSERT(A3 == X.skipUnknownElements());
        ASSERT(A4 == X.validateInputIsUtf8());
        ASSERT(A5 == X.validateRootTag());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: `x` <op> `w`, `x`." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object `y` (init. to `A`)."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;

        mY.setMaxDepth(A1);
        mY.setFormattingMode(A2);
        mY.setSkipUnknownElements(A3);
        mY.setValidateInputIsUtf8(A4);
        mY.setValidateRootTag(A4);

        if (veryVerbose) cout << "\ta. Check initial value of `y`." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.maxDepth());
        ASSERT(A2 == Y.formattingMode());
        ASSERT(A3 == Y.skipUnknownElements());
        ASSERT(A4 == Y.validateInputIsUtf8());
        ASSERT(A5 == Y.validateRootTag());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: `y` <op> `w`, `x`, `y`" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object `z` (copy from `y`)."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of `z`." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        mZ.setMaxDepth(A1);
        mZ.setFormattingMode(A2);
        mZ.setSkipUnknownElements(A3);
        mZ.setValidateInputIsUtf8(A4);
        mZ.setValidateRootTag(A4);

        ASSERT(A1 == Z.maxDepth());
        ASSERT(A2 == Z.formattingMode());
        ASSERT(A3 == Z.skipUnknownElements());
        ASSERT(A4 == Z.validateInputIsUtf8());
        ASSERT(A5 == Z.validateRootTag());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: `z` <op> `w`, `x`, `y`, `z`." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set `z` to `D` (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setMaxDepth(D1);
        mZ.setFormattingMode(D2);
        mZ.setSkipUnknownElements(D3);
        mZ.setValidateInputIsUtf8(D4);
        mZ.setValidateRootTag(D4);

        if (veryVerbose) cout << "\ta. Check new value of `z`." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.maxDepth());
        ASSERT(D2 == Z.formattingMode());
        ASSERT(D3 == Z.skipUnknownElements());
        ASSERT(D4 == Z.validateInputIsUtf8());
        ASSERT(D5 == Z.validateRootTag());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: `z` <op> `w`, `x`, `y`, `z`." << endl;

        ASSERTV(Z, W, 1 == (Z == W));        ASSERTV(Z, W, 0 == (Z != W));
        ASSERTV(Z, X, 0 == (Z == X));        ASSERTV(Z, X, 1 == (Z != X));
        ASSERTV(Z, Y, 0 == (Z == Y));        ASSERTV(Z, Y, 1 == (Z != Y));
        ASSERTV(Z, Z, 1 == (Z == Z));        ASSERTV(Z, Z, 0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign `w` from `x`."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of `w`." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.maxDepth());
        ASSERT(A2 == W.formattingMode());
        ASSERT(A3 == W.skipUnknownElements());
        ASSERT(A4 == W.validateInputIsUtf8());
        ASSERT(A5 == W.validateRootTag());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: `w` <op> `w`, `x`, `y`, `z`." << endl;

        ASSERTV(W, W, 1 == (W == W));        ASSERTV(W, W, 0 == (W != W));
        ASSERTV(W, X, 1 == (W == X));        ASSERTV(W, X, 0 == (W != X));
        ASSERTV(W, Y, 1 == (W == Y));        ASSERTV(W, Y, 0 == (W != Y));
        ASSERTV(W, Z, 0 == (W == Z));        ASSERTV(W, Z, 1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign `w` from `z`."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of `w`." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.maxDepth());
        ASSERT(D2 == W.formattingMode());
        ASSERT(D3 == W.skipUnknownElements());
        ASSERT(D4 == W.validateInputIsUtf8());
        ASSERT(D5 == W.validateRootTag());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: `x` <op> `w`, `x`, `y`, `z`." << endl;

        ASSERTV(W, W, 1 == (W == W));        ASSERTV(W, W, 0 == (W != W));
        ASSERTV(W, X, 0 == (W == X));        ASSERTV(W, X, 1 == (W != X));
        ASSERTV(W, Y, 0 == (W == Y));        ASSERTV(W, Y, 1 == (W != Y));
        ASSERTV(W, Z, 1 == (W == Z));        ASSERTV(W, Z, 0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign `x` from `x` (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of `x`." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.maxDepth());
        ASSERT(A2 == X.formattingMode());
        ASSERT(A3 == X.skipUnknownElements());
        ASSERT(A4 == X.validateInputIsUtf8());
        ASSERT(A5 == X.validateRootTag());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: `x` <op> `w`, `x`, `y`, `z`." << endl;

        ASSERTV(X, W, 0 == (X == W));        ASSERTV(X, W, 1 == (X != W));
        ASSERTV(X, X, 1 == (X == X));        ASSERTV(X, X, 0 == (X != X));
        ASSERTV(X, Y, 1 == (X == Y));        ASSERTV(X, Y, 0 == (X != Y));
        ASSERTV(X, Z, 0 == (X == Z));        ASSERTV(X, Z, 1 == (X != Z));

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
// Copyright 2024 Bloomberg Finance L.P.
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
