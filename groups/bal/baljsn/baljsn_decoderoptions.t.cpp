// baljsn_decoderoptions.t.cpp                                        -*-C++-*-

#include <baljsn_decoderoptions.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

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
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setMaxDepth'
//: o 'setSkipUnknownElements'
//
// Basic Accessors:
//: o 'maxDepth'
//: o 'skipUnknownElements'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- 'swap' is not implemented for this class.
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//: o No memory is allocated from any allocator.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baljsn::DecoderOptions();
// [ 7] baljsn::DecoderOptions(const baljsn::DecoderOptions& original);
// [ 2] ~baljsn::DecoderOptions();
//
// MANIPULATORS
// [ 9] operator=(const baljsn::DecoderOptions& rhs);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 3] setMaxDepth(int value);
// [ 3] setSkipUnknownElements(bool value);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] int  maxDepth() const;
// [ 4] bool skipUnknownElements() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baljsn::DecoderOptions& lhs, rhs);
// [ 6] bool operator!=(const baljsn::DecoderOptions& lhs, rhs);
// [ 5] operator<<(ostream& s, const baljsn::DecoderOptions& d);
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

typedef baljsn::DecoderOptions Obj;

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
    bool        d_skipUnknownElements;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  MAX_DEPTH   SKIP_ELEMS
    //----  ---------   ----------

    // default (must be first)
    { L_,        32,    true },

    // 'maxDepth'
    { L_,         1,  false },
    { L_,   INT_MAX,  false },

    // 'skipUnknownElements'
    { L_,         0,   true },

    // other
    { L_,         1,   true },
    { L_,   INT_MAX,   true },
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

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
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
// First, we default-construct a 'baljsn::DecoderOptions' object:
//..
    const int  MAX_DEPTH             = 10;
    const bool SKIP_UNKNOWN_ELEMENTS = false;

    baljsn::DecoderOptions options;
    ASSERT(32   == options.maxDepth());
    ASSERT(true == options.skipUnknownElements());
//..
// Next, we populate that object to decode using a different 'maxDepth' value
// and 'skipUnknownElements' value:
//..
    options.setMaxDepth(MAX_DEPTH);
    ASSERT(MAX_DEPTH == options.maxDepth());

    options.setSkipUnknownElements(SKIP_UNKNOWN_ELEMENTS);
    ASSERT(SKIP_UNKNOWN_ELEMENTS == options.skipUnknownElements());
//..
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
        //  Reserved for 'swap' testing.
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
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-3.2):  (C-1, 3..4)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, the source object, 'Z', is a reference to the target
        //:   object, 'mX', and both 'mX' and 'ZZ' are initialized to have the
        //:   value 'V'.  For each row (representing a distinct object value,
        //:   'V') in the table described in P-2:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-3)
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-5)
        //
        // Testing:
        //   operator=(const baljsn::DecoderOptions& rhs);
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
            const int   LINE1    = DATA[ti].d_line;
            const int   DEPTH1  = DATA[ti].d_maxDepth;
            const int   SKIP1     = DATA[ti].d_skipUnknownElements;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setMaxDepth(DEPTH1);
            mZ.setSkipUnknownElements(SKIP1);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setMaxDepth(DEPTH1);
            mZZ.setSkipUnknownElements(SKIP1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2    = DATA[tj].d_line;
                const int   DEPTH2  = DATA[tj].d_maxDepth;
                const int   SKIP2     = DATA[tj].d_skipUnknownElements;

                Obj mX;  const Obj& X = mX;
                mX.setMaxDepth(DEPTH2);
                mX.setSkipUnknownElements(SKIP2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                LOOP4_ASSERT(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            if (verbose) cout << "Testing self-assignment" << endl;

            {
                Obj mX;  const Obj& X = mX;
                mX.setMaxDepth(DEPTH1);
                mX.setSkipUnknownElements(SKIP1);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setMaxDepth(DEPTH1);
                mZZ.setSkipUnknownElements(SKIP1);

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
        //  Reserved for 'swap' testing.
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
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Use the copy constructor to create an object 'X',
        //:     supplying it the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:     (C-1, 3)
        //:
        //:     1 The newly constructed object, 'X', has the same value as 'Z'.
        //:       (C-1)
        //:
        //:     2 'Z' still has the same value as 'ZZ'.  (C-3)
        //
        // Testing:
        //   baljsn::DecoderOptions(const baljsn::DecoderOptions& o);
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
            const int   LINE    = DATA[ti].d_line;
            const int   DEPTH   = DATA[ti].d_maxDepth;
            const int   SKIP    = DATA[ti].d_skipUnknownElements;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setMaxDepth(DEPTH);
            mZ.setSkipUnknownElements(SKIP);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setMaxDepth(DEPTH);
            mZZ.setSkipUnknownElements(SKIP);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE, Obj(), Z,
                             Obj() == Z)
                firstFlag = false;
            }

            // Verify the value of the object.

            LOOP3_ASSERT(LINE,  Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }  // end foreach row
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutative)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutative)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..8)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutative property and expected return value for
        //:       both '==' and '!='.  (C-1..3, 6..8)
        //
        // Testing:
        //   bool operator==(const baljsn::DecoderOptions& lhs, rhs);
        //   bool operator!=(const baljsn::DecoderOptions& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace baljsn;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;
        // ---------------
        // Attribute Types
        // ---------------

        typedef int   T1;        // 'maxDepth'
        typedef bool  T2;        // 'skipUnknownElements'

                      // ------------------------------
                      // Attribute 1 Values: 'maxDepth'
                      // ------------------------------

        const T1 A1 = INT_MAX;               // baseline
        const T1 B1 = 0;

                // -----------------------------------------
                // Attribute 2 Values: 'skipUnknownElements'
                // -----------------------------------------

        const T2 A2 = true;                  // baseline
        const T2 B2 = false;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int   d_line;        // source line number
            int   d_maxDepth;
            bool  d_skipUnknownElements;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute value
        // (Bi).

        //LINE  DEPTH   SKIP
        //----  -----   ----

        { L_,       A1,   A2   },          // baseline

        { L_,       B1,   A2   },
        { L_,       A1,   B2   },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1    = DATA[ti].d_line;
            const int   DEPTH1  = DATA[ti].d_maxDepth;
            const int   SKIP1     = DATA[ti].d_skipUnknownElements;

            if (veryVerbose) { T_ P_(LINE1) P_(DEPTH1) P_(SKIP1)  }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mX;  const Obj& X = mX;

                mX.setMaxDepth(DEPTH1);
                mX.setSkipUnknownElements(SKIP1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2    = DATA[tj].d_line;
                const int   DEPTH2  = DATA[tj].d_maxDepth;
                const int   SKIP2     = DATA[tj].d_skipUnknownElements;

                if (veryVerbose) { T_ P_(LINE1) P_(DEPTH2) P_(SKIP2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setMaxDepth(DEPTH1);
                mX.setSkipUnknownElements(SKIP1);

                mY.setMaxDepth(DEPTH2);
                mY.setSkipUnknownElements(SKIP2);

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
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 6)
        //:
        //: 2 Using the table-driven technique, define twelve carefully
        //:   selected combinations of (two) object values ('A' and 'B'),
        //:   having distinct values for each corresponding salient attribute,
        //:   and various values for the two formatting parameters, along with
        //:   the expected output
        //:     ( 'value' x  'level'   x 'skipUnknownElements' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //: 3 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:   1 Using a 'const' 'Obj', supply each object value and pair of
        //:     formatting parameters to 'print', unless the parameters are,
        //:     arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:     invoked instead.
        //:
        //:   2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:   3 Verify the address of what is returned is that of the
        //:     supplied stream.  (C-5, 7)
        //:
        //:   4 Compare the contents captured in P-2.2.2 with what is
        //:     expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const baljsn::DecoderOptions& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OUTPUT OPERATOR" << endl
                          << "=================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace baljsn;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spl;

            int         d_maxDepth;
            bool        d_skipUnknownElements;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1 } --> 3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  MD   SUE  EXP
        //---- - ---  ---  ---  ---

        { L_,  0,  0, 89, true,  "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "skipUnknownElements = true"                NL
                                 "]"                                         NL
                                                                             },

        { L_,  0,  1, 89, true,  "["                                         NL
                                 " maxDepth = 89"                            NL
                                 " skipUnknownElements = true"               NL
                                 "]"                                         NL
                                                                             },

        { L_,  0, -1, 89, false, "["                                         SP
                                 "maxDepth = 89"                             SP
                                 "skipUnknownElements = false"               SP
                                 "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 } --> 6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  MD   SUE  EXP
        //---- - ---  ---  ---  ---

        { L_,  3,  0, 89, true,  "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "skipUnknownElements = true"                NL
                                 "]"                                         NL
                                                                             },

        { L_,  3,  2, 89, false, "      ["                                   NL
                                 "        maxDepth = 89"                     NL
                                 "        skipUnknownElements = false"       NL
                                 "      ]"                                   NL
                                                                             },

        { L_,  3, -2, 89, false, "      ["                                   SP
                                 "maxDepth = 89"                             SP
                                 "skipUnknownElements = false"               SP
                                 "]"
                                                                             },

        { L_, -3,  0, 89, true,  "["                                         NL
                                 "maxDepth = 89"                             NL
                                 "skipUnknownElements = true"                NL
                                 "]"                                         NL
                                                                             },

        { L_, -3,  2, 89, false, "["                                         NL
                                 "        maxDepth = 89"                     NL
                                 "        skipUnknownElements = false"       NL
                                 "      ]"                                   NL
                                                                             },

        { L_, -3, -2, 89, true,  "["                                         SP
                                 "maxDepth = 89"                             SP
                                 "skipUnknownElements = true"                SP
                                 "]"
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  MD   SUE  EXP
        //---- - ---  ---  ---  ---

        { L_,  2,  3, 89, true,  "      ["                                   NL
                                 "         maxDepth = 89"                    NL
                                 "         skipUnknownElements = true"       NL
                                 "      ]"                                   NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 } x { -9 } --> 2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  MD   SUE  EXP
        //---- - ---  ---  ---  ---

        { L_, -9, -9,  89, true,  "[ 89 true ]"                              },

        { L_, -9, -9,   7, false, "[ 7 false ]"                              },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         S      = DATA[ti].d_spl;

                const int         MAX_DEPTH  = DATA[ti].d_maxDepth;
                const int         SKIP_ELEMS = DATA[ti].d_skipUnknownElements;

                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(MAX_DEPTH) P_(SKIP_ELEMS)}

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                mX.setMaxDepth(MAX_DEPTH);
                mX.setSkipUnknownElements(SKIP_ELEMS);

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
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):  (C-1)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //
        // Testing:
        //   int  maxDepth() const;
        //   bool skipUnknownElements() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        // Attribute Types

        typedef int   T1;        // 'maxDepth'
        typedef bool  T2;        // 'skipUnknownElements'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

          // -----------------------------------------------------
          // 'D' values: These are the default-constructed values.
          // -----------------------------------------------------

        const int   D1   = 32;                    // 'maxDepth'
        const int   D2   = true;                  // 'skipUnknownElements'

                       // ----------------------------
                       // 'A' values: Boundary values.
                       // ----------------------------

        const int   A1   = INT_MAX;              // 'maxDepth'
        const int   A2   = false;                // 'skipUnknownElements'

        if (verbose) cout << "\nCreate an object." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& maxDepth = X.maxDepth();
            LOOP2_ASSERT(D1, maxDepth, D1 == maxDepth);

            const T2& skipUnknownElements = X.skipUnknownElements();
            LOOP2_ASSERT(D2, skipUnknownElements, D2 == skipUnknownElements);
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(maxDepth) }
        {
            mX.setMaxDepth(A1);

            const T1& maxDepth = X.maxDepth();
            LOOP2_ASSERT(A1, maxDepth, A1 == maxDepth);
        }

        if (veryVerbose) { T_ Q(skipUnknownElements) }
        {
            mX.setSkipUnknownElements(A2);

            const T2& skipUnknownElements = X.skipUnknownElements();
            LOOP2_ASSERT(A2, skipUnknownElements, A2 == skipUnknownElements);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //: 1 Any argument can be 'const'.
        //:
        //: 2 Each attribute is modifiable independently.
        //:
        //: 3 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: 'D', 'A',
        //:   and 'B'.  'D' values correspond to the attribute values, and 'A'
        //:   and 'B' values are chosen to be distinct boundary values where
        //:   possible.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-1, 3)
        //:
        //: 4 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-2)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   setMaxDepth(int value);
        //   setSkipUnknownElements(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int   D1   = 32;                   // 'maxDepth'
        const bool  D2   = true;                 // 'skipUnknownElements'

        // 'A' values.

        const int   A1   = 1;                    // 'maxDepth'
        const int   A2   = false;                // 'skipUnknownElements'

        // 'B' values.

        const int   B1   = INT_MAX;              // 'maxDepth'
        const int   B2   = true;                 // 'skipUnknownElements'

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // ----------
        // 'maxDepth'
        // ----------
        {
            mX.setMaxDepth(A1);
            ASSERT(A1 == X.maxDepth());
            ASSERT(D2 == X.skipUnknownElements());

            mX.setMaxDepth(B1);
            ASSERT(B1 == X.maxDepth());
            ASSERT(D2 == X.skipUnknownElements());

            mX.setMaxDepth(D1);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.skipUnknownElements());
        }

        // ---------------------
        // 'skipUnknownElements'
        // ---------------------
        {
            mX.setSkipUnknownElements(A2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(A2 == X.skipUnknownElements());

            mX.setSkipUnknownElements(B2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(B2 == X.skipUnknownElements());

            mX.setSkipUnknownElements(D2);
            ASSERT(D1 == X.maxDepth());
            ASSERT(D2 == X.skipUnknownElements());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            mX.setMaxDepth(A1);
            mX.setSkipUnknownElements(A2);

            ASSERT(A1 == X.maxDepth());
            ASSERT(A2 == X.skipUnknownElements());

                 // ---------------------------------------
                 // Set all attributes to their 'B' values.
                 // ---------------------------------------

            mX.setMaxDepth(B1);

            ASSERT(B1 == X.maxDepth());
            ASSERT(A2 == X.skipUnknownElements());

            mX.setSkipUnknownElements(B2);

            ASSERT(B1 == X.maxDepth());
            ASSERT(B2 == X.skipUnknownElements());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tMaxDepth" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setMaxDepth(-1));
                ASSERT_SAFE_PASS(obj.setMaxDepth( 0));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //
        // Plan:
        //: 1 Use the default constructor to create an object 'X'.
        //:
        //: 2 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //
        // Testing:
        //   baljsn::DecoderOptions();
        //   ~baljsn::DecoderOptions();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int   D1   = 32;                   // 'maxDepth'
        const bool  D2   = true;                 // 'skipUnknownElements'

        if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout << "Verify the object's attribute values." << endl;

                  // -------------------------------------
                  // Verify the object's attribute values.
                  // -------------------------------------

        LOOP2_ASSERT(D1, X.maxDepth(), D1 == X.maxDepth());
        LOOP2_ASSERT(D2, X.skipUnknownElements(),
                     D2 == X.skipUnknownElements());
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
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute Types

        typedef int    T1;        // 'maxDepth'
        typedef bool   T2;        // 'skipUnknownElements'

        // Attribute 1 Values: 'maxDepth'

        const T1 D1 = 32;        // default value
        const T1 A1 =  0;

        // Attribute 2 Values: 'skipUnknownElements'

        const T2 D2 = true;    // default value
        const T2 A2 = false;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.maxDepth());
        ASSERT(D2 == W.skipUnknownElements());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.maxDepth());
        ASSERT(D2 == X.skipUnknownElements());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setMaxDepth(A1);
        mX.setSkipUnknownElements(A2);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.maxDepth());
        ASSERT(A2 == X.skipUnknownElements());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;
        mY.setMaxDepth(A1);
        mY.setSkipUnknownElements(A2);

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.maxDepth());
        ASSERT(A2 == Y.skipUnknownElements());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.maxDepth());
        ASSERT(A2 == Z.skipUnknownElements());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setMaxDepth(D1);
        mZ.setSkipUnknownElements(D2);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.maxDepth());
        ASSERT(D2 == Z.skipUnknownElements());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.maxDepth());
        ASSERT(A2 == W.skipUnknownElements());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.maxDepth());
        ASSERT(D2 == W.skipUnknownElements());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.maxDepth());
        ASSERT(A2 == X.skipUnknownElements());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

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
