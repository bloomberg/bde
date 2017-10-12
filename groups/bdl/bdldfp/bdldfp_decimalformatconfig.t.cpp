// bdldfp_decimalformatconfig.t.cpp                                   -*-C++-*-
#include <bdldfp_decimalformatconfig.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a simple in-core value-semantic
// attribute class.  The 'class' supports a subset of the value-semantic
// functionality: Primary Manipulators and Basic Accessors.
//
// Primary Manipulators:
//: o 'setPrecision'
//: o 'setStyle'
//: o 'setSign'
//: o 'setInfinity'
//: o 'setNan'
//: o 'setSNan'
//: o 'setDecimalPoint'
//: o 'setExponent'
//
// Basic Accessors:
//: o 'precision'
//: o 'style'
//: o 'sign'
//: o 'infinity'
//: o 'nan'
//: o 'sNan'
//: o 'decimalPoint'
//: o 'exponent'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 5] -- 'print' and 'operator<<' for this class.
//: o [ 8] -- 'swap' is not implemented for this class.
//: o [10] -- BDEX streaming is not (yet) implemented for this class.
//
// ----------------------------------------------------------------------------
// CREATORS
// [2] bdldfp::DecimalFormatConfig();
// [7] bdldfp::DecimalFormatConfig(const DecimalFormatConfig&);
// [3] bdldfp::DecimalFormatConfig(int, Style, Sign, str, str, str, char,char);
// [2] ~bdldfp::DecimalFormatConfig();
//
// MANIPULATORS
// [9] operator=(const bdldfp::DecimalFormatConfig&);
// [3] setPrecision(int);
// [3] setStyle(Style);
// [3] setSign(Sign);
// [3] setInfinity(const char *);
// [3] setNan(const char *);
// [3] setSNan(const char *);
// [3] decimalPoint(char);
// [3] exponent(char);
//
// ACCESSORS
// [4] int precision() const;
// [4] Style style() const;
// [4] Sign sign() const;
// [4] const char *infinity() const;
// [4] const char *nan() const;
// [4] const char *sNan() const;
// [4] char decimalPoint() const;
// [4] char exponent() const;
//
// FREE OPERATORS
// [6] bool operator==(const bdldfp::DecimalFormatConfig& lhs, rhs);
// [6] bool operator!=(const bdldfp::DecimalFormatConfig& lhs, rhs);
// ----------------------------------------------------------------------------
// [1] BREATHING TEST

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

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;
static bslma::TestAllocator *pa;

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;
typedef BDEC::DecimalFormatConfig Obj;

struct DefaultDataRow {
    int         d_line;          // source line number
    int         d_precision;     // precision
    Obj::Style  d_style;         // formatting style
    Obj::Sign   d_sign;          // sign character
    const char *d_infinityText;  // infinity representation
    const char *d_nanText;       // NaN representation
    const char *d_sNanText;      // signaling NaN representation
    char        d_decimalPoint;  // decimal point character
    char        d_exponent;      // exponent character
};

static const DefaultDataRow DEFAULT_DATA[] = {

#define SCI Obj::e_SCIENTIFIC
#define FXD Obj::e_FIXED
#define NAT Obj::e_NATURAL

#define NEG Obj::e_NEGATIVE_ONLY
#define ALS Obj::e_ALWAYS

//LINE PRS  STYLE SIGN INF NAN SNAN PNT EXP
//---- ---  ----- ---- --- --- ---- --- ---
// default (must be first)

{ L_, 15,      SCI, NEG, "inf", "nan", "snan", '.', 'E'},

// 'precision'
{ L_, 0,       FXD, ALS, "",    "",    "",     '.', 'E'},
{ L_, INT_MAX, FXD, ALS, "",    "",    "",     '.', 'E'},

// 'style'
{ L_, 0,       SCI, NEG, "",    "",    "",     '.', 'E'},
{ L_, INT_MAX, NAT, ALS, "",    "",    "",     '.', 'E'},

// 'sign'
{ L_, 0,       SCI, ALS, "",    "",    "",     '.', 'E'},
{ L_, INT_MAX, NAT, NEG, "",    "",    "",     '.', 'E'},

// 'infinity'
{ L_, 0,       SCI, ALS, "inf", "",    "",     '.', 'E'},
{ L_, INT_MAX, NAT, NEG, "INF", "",    "",     '.', 'E'},

// 'nan'
{ L_, 0,       SCI, ALS, "inf", "nan", "",     '.', 'E'},
{ L_, INT_MAX, NAT, NEG, "INF", "NAN", "",     '.', 'E'},

// 'snan'
{ L_, 0,       SCI, ALS, "inf", "nan", "snan", '.', 'E'},
{ L_, INT_MAX, SCI, NEG, "INF", "NAN", "SNAN", '.', 'E'},

// 'point'
{ L_, 0,       FXD, ALS, "inf", "nan", "snan", '.', 'E'},
{ L_, INT_MAX, FXD, NEG, "INF", "NAN", "SNAN", ',', 'E'},

// 'exponent'
{ L_, 0,       NAT, ALS, "inf", "nan", "snan", '.', 'E'},
{ L_, INT_MAX, NAT, NEG, "INF", "NAN", "SNAN", ',', 'e'},
};

#undef SCI
#undef FXD
#undef NAT

#undef NEG
#undef ALS

const int DEFAULT_NUM_DATA = static_cast<int>(
                                   sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

    cout.precision(35);

    switch (test) { case 0:
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
        //  Reserved for bdex streaming test.
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
        //   operator=(const bdldfp::DecimalFormatConfig& rhs);
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
            const int         LINE1 = DATA[ti].d_line;
            const int         PRS1  = DATA[ti].d_precision;
            const Obj::Style  STL1  = DATA[ti].d_style;
            const Obj::Sign   SGN1  = DATA[ti].d_sign;
            const char       *INF1  = DATA[ti].d_infinityText;
            const char       *QNAN1 = DATA[ti].d_nanText;
            const char       *SNAN1 = DATA[ti].d_sNanText;
            const char        PNT1  = DATA[ti].d_decimalPoint;
            const char        EXP1  = DATA[ti].d_exponent;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setPrecision(PRS1);
            mZ.setStyle(STL1);
            mZ.setSign(SGN1);
            mZ.setInfinity(INF1);
            mZ.setNan(QNAN1);
            mZ.setSNan(SNAN1);
            mZ.setDecimalPoint(PNT1);
            mZ.setExponent(EXP1);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setPrecision(PRS1);
            mZZ.setStyle(STL1);
            mZZ.setSign(SGN1);
            mZZ.setInfinity(INF1);
            mZZ.setNan(QNAN1);
            mZZ.setSNan(SNAN1);
            mZZ.setDecimalPoint(PNT1);
            mZZ.setExponent(EXP1);

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP1_ASSERT(LINE1, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const int         PRS2  = DATA[tj].d_precision;
                const Obj::Style  STL2  = DATA[tj].d_style;
                const Obj::Sign   SGN2  = DATA[tj].d_sign;
                const char       *INF2  = DATA[tj].d_infinityText;
                const char       *QNAN2 = DATA[tj].d_nanText;
                const char       *SNAN2 = DATA[tj].d_sNanText;
                const char        PNT2  = DATA[tj].d_decimalPoint;
                const char        EXP2  = DATA[tj].d_exponent;

                Obj mX;  const Obj& X = mX;
                mX.setPrecision(PRS2);
                mX.setStyle(STL2);
                mX.setSign(SGN2);
                mX.setInfinity(INF2);
                mX.setNan(QNAN2);
                mX.setSNan(SNAN2);
                mX.setDecimalPoint(PNT2);
                mX.setExponent(EXP2);

                LOOP2_ASSERT(LINE1, LINE2, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP2_ASSERT(LINE1, LINE2,  Z == X);
                LOOP2_ASSERT(LINE1, LINE2, mR == &mX);

                LOOP2_ASSERT(LINE1, LINE2, ZZ == Z);
            }

            if (verbose) cout << "Testing self-assignment" << endl;

            {
                Obj mX;
                mX.setPrecision(PRS1);
                mX.setStyle(STL1);
                mX.setSign(SGN1);
                mX.setInfinity(INF1);
                mX.setNan(QNAN1);
                mX.setSNan(SNAN1);
                mX.setDecimalPoint(PNT1);
                mX.setExponent(EXP1);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setPrecision(PRS1);
                mZZ.setStyle(STL1);
                mZZ.setSign(SGN1);
                mZZ.setInfinity(INF1);
                mZZ.setNan(QNAN1);
                mZZ.setSNan(SNAN1);
                mZZ.setDecimalPoint(PNT1);
                mZZ.setExponent(EXP1);

                const Obj& Z = mX;

                LOOP1_ASSERT(LINE1, ZZ == Z);

                Obj *mR = &(mX = Z);
                LOOP1_ASSERT(LINE1, ZZ == Z);
                LOOP1_ASSERT(LINE1, mR == &mX);
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
        //   bdldfp::DecimalFormatConfig(const bdldfp::DecimalFormatConfig& o);
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
            const int         LINE = DATA[ti].d_line;
            const int         PRS  = DATA[ti].d_precision;
            const Obj::Style  STL  = DATA[ti].d_style;
            const Obj::Sign   SGN  = DATA[ti].d_sign;
            const char       *INF  = DATA[ti].d_infinityText;
            const char       *QNAN = DATA[ti].d_nanText;
            const char       *SNAN = DATA[ti].d_sNanText;
            const char        PNT  = DATA[ti].d_decimalPoint;
            const char        EXP  = DATA[ti].d_exponent;

            Obj mZ;  const Obj& Z = mZ;
            mZ.setPrecision(PRS);
            mZ.setStyle(STL);
            mZ.setSign(SGN);
            mZ.setInfinity(INF);
            mZ.setNan(QNAN);
            mZ.setSNan(SNAN);
            mZ.setDecimalPoint(PNT);
            mZ.setExponent(EXP);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setPrecision(PRS);
            mZZ.setStyle(STL);
            mZZ.setSign(SGN);
            mZZ.setInfinity(INF);
            mZZ.setNan(QNAN);
            mZZ.setSNan(SNAN);
            mZZ.setDecimalPoint(PNT);
            mZZ.setExponent(EXP);

            Obj mX(Z);  const Obj& X = mX;

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP1_ASSERT(LINE, Obj() == Z)
                firstFlag = false;
            }

            // Verify the value of the object.

            LOOP1_ASSERT(LINE, Z == X);

            // Verify that the value of 'Z' has not changed.

            LOOP1_ASSERT(LINE, ZZ == Z);

        }
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
        //: 3 'true  == (X == X)'  (i.e., identity)
        //:
        //: 4 'false == (X != X)'  (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:10 The equality operator's signature and return type are standard.
        //:
        //:11 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-8..11)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..7)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..2, 5..7)
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
        //:       both '==' and '!='.  (C-1..2, 5..7)
        //
        // Testing:
        //   bool operator==(const bdldfp::DecimalFormatConfig& lhs, rhs);
        //   bool operator!=(const bdldfp::DecimalFormatConfig& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdldfp;
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

        typedef int         T1;   // 'precision'
        typedef Obj::Style  T2;   // 'style'
        typedef Obj::Sign   T3;   // 'sign'
        typedef const char* T4;   // 'infinity'
        typedef const char* T5;   // 'nan'
        typedef const char* T6;   // 'snan'
        typedef char        T7;   // 'point'
        typedef char        T8;   // 'exponent

                 // -------------------------------
                 // Attribute 1 Values: 'precision'
                 // -------------------------------

        const T1 A1 = 1;               // baseline
        const T1 B1 = INT_MAX;

                 // ---------------------------
                 // Attribute 2 Values: 'style'
                 // ---------------------------

        const T2 A2 = Obj::e_SCIENTIFIC;    // baseline
        const T2 B2 = Obj::e_FIXED;

                 // --------------------------
                 // Attribute 3 Values: 'sign'
                 // --------------------------

        const T3 A3 = Obj::e_NEGATIVE_ONLY; // baseline
        const T3 B3 = Obj::e_ALWAYS;

                 // ------------------------------
                 // Attribute 4 Values: 'infinity'
                 // ------------------------------

        const T4 A4 = "inf";           // baseline
        const T4 B4 = "Inf";

                 // -------------------------
                 // Attribute 5 Values: 'nan'
                 // -------------------------

        const T5 A5 = "nan";           // baseline
        const T5 B5 = "NaN";

                 // -------------------------
                 // Attribute 6 Values: 'nan'
                 // -------------------------

        const T6 A6 = "snan";          // baseline
        const T6 B6 = "sNaN";

                 // --------------------------
                 // Attribute 7 Values: 'point'
                 // --------------------------

        const T7 A7 = '.';             // baseline
        const T7 B7 = ',';

                 // ------------------------------
                 // Attribute 8 Values: 'exponent'
                 // ------------------------------

        const T8 A8 = 'E';             // baseline
        const T8 B8 = 'e';

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;          // source line number
            int         d_precision;     // precision
            Obj::Style  d_style;         // formatting style
            Obj::Sign   d_sign;          // sign character
            const char *d_infinityText;  // infinity representation
            const char *d_nanText;       // NaN representation
            const char *d_sNanText;      // signaling NaN representation
            char        d_decimalPoint;  // decimal point character
            char        d_exponent;      // exponent character
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute value
        // (Bi).

       //LINE PRS  STYLE SIGN INF NAN SNAN PNT EXP
       //---- ---  ----- ---- --- --- ---- --- ---
        { L_, A1,  A2,   A3,  A4, A5,  A6, A7, A8 },  // baseline

        { L_, B1,  A2,   A3,  A4, A5,  A6, A7, A8 },
        { L_, A1,  B2,   A3,  A4, A5,  A6, A7, A8 },
        { L_, A1,  A2,   B3,  A4, A5,  A6, A7, A8 },
        { L_, A1,  A2,   A3,  B4, A5,  A6, A7, A8 },
        { L_, A1,  A2,   A3,  A4, B5,  A6, A7, A8 },
        { L_, A1,  A2,   A3,  A4, A5,  B6, A7, A8 },
        { L_, A1,  A2,   A3,  A4, A5,  A6, B7, A8 },
        { L_, A1,  A2,   A3,  A4, A5,  A6, A7, B8 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1 = DATA[ti].d_line;
            const int         PRS1  = DATA[ti].d_precision;
            const Obj::Style  STL1  = DATA[ti].d_style;
            const Obj::Sign   SGN1  = DATA[ti].d_sign;
            const char       *INF1  = DATA[ti].d_infinityText;
            const char       *QNAN1 = DATA[ti].d_nanText;
            const char       *SNAN1 = DATA[ti].d_sNanText;
            const char        PNT1  = DATA[ti].d_decimalPoint;
            const char        EXP1  = DATA[ti].d_exponent;

            if (veryVerbose) { T_ P_(LINE1) P_(PRS1)  P_(STL1)  P_(SGN1)
                                  P_(INF1)  P_(QNAN1) P_(SNAN1) P_(PNT1)
                                  P(EXP1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mX;  const Obj& X = mX;

                mX.setPrecision(PRS1);
                mX.setStyle(STL1);
                mX.setSign(SGN1);
                mX.setInfinity(INF1);
                mX.setNan(QNAN1);
                mX.setSNan(SNAN1);
                mX.setDecimalPoint(PNT1);
                mX.setExponent(EXP1);

                LOOP1_ASSERT(LINE1,   X == X);
                LOOP1_ASSERT(LINE1, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const int         PRS2  = DATA[tj].d_precision;
                const Obj::Style  STL2  = DATA[tj].d_style;
                const Obj::Sign   SGN2  = DATA[tj].d_sign;
                const char       *INF2  = DATA[tj].d_infinityText;
                const char       *QNAN2 = DATA[tj].d_nanText;
                const char       *SNAN2 = DATA[tj].d_sNanText;
                const char        PNT2  = DATA[tj].d_decimalPoint;
                const char        EXP2  = DATA[tj].d_exponent;
                if (veryVerbose) { T_ P_(LINE2) P_(PRS2)  P_(STL2)  P_(SGN2)
                                      P_(INF2)  P_(QNAN2) P_(SNAN2) P_(PNT2)
                                      P(EXP2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;

                mX.setPrecision(PRS1);
                mX.setStyle(STL1);
                mX.setSign(SGN1);
                mX.setInfinity(INF1);
                mX.setNan(QNAN1);
                mX.setSNan(SNAN1);
                mX.setDecimalPoint(PNT1);
                mX.setExponent(EXP1);

                mY.setPrecision(PRS2);
                mY.setStyle(STL2);
                mY.setSign(SGN2);
                mY.setInfinity(INF2);
                mY.setNan(QNAN2);
                mY.setSNan(SNAN2);
                mY.setDecimalPoint(PNT2);
                mY.setExponent(EXP2);

                // Verify value, and commutative.

                LOOP2_ASSERT(LINE1, LINE2,  EXP == (X == Y));
                LOOP2_ASSERT(LINE1, LINE2,  EXP == (Y == X));

                LOOP2_ASSERT(LINE1, LINE2, !EXP == (X != Y));
                LOOP2_ASSERT(LINE1, LINE2, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT OPERATOR
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'print' and 'operator<<' testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OUTPUT OPERATOR" << endl
                          << "=================================" << endl;
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
        //   int         precision() const;
        //   Style       style() const;
        //   Sign        sign() const;
        //   const char *infinity() const;
        //   const char *nan() const;
        //   const char *sNan() const;
        //   char        decimalPoint() const;
        //   char        exponent() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        //----------------
        // Attribute Types
        //----------------

        typedef int         T1;   // 'precision'
        typedef Obj::Style  T2;   // 'style'
        typedef Obj::Sign   T3;   // 'sign'
        typedef const char* T4;   // 'infinity'
        typedef const char* T5;   // 'nan'
        typedef const char* T6;   // 'snan'
        typedef char        T7;   // 'point'
        typedef char        T8;   // 'exponent

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

          // -----------------------------------------------------
          // 'D' values: These are the default-constructed values.
          // -----------------------------------------------------

        const int         D1 = 15;                    // 'precision'
        const Obj::Style  D2 = Obj::e_SCIENTIFIC;     // 'style'
        const Obj::Sign   D3 = Obj::e_NEGATIVE_ONLY;  // 'sign'
        const char       *D4 = "inf";                 // 'infinity'
        const char       *D5 = "nan";                 // 'nan'
        const char       *D6 = "snan";                // 'snan'
        const char        D7 = '.';                   // 'point'
        const char        D8 = 'E';                   // 'exponent

                       // ----------------------------
                       // 'A' values: Boundary values.
                       // ----------------------------

        const int         A1 = INT_MAX;               // 'precision'
        const Obj::Style  A2 = Obj::e_NATURAL;        // 'style'
        const Obj::Sign   A3 = Obj::e_ALWAYS;         // 'sign'
        const char       *A4 = "";                    // 'infinity'
        const char       *A5 = "";                    // 'nan'
        const char       *A6 = "";                    // 'snan'
        const char        A7 = ',';                   // 'point'
        const char        A8 = 'e';                   // 'exponent

        if (verbose) cout << "\nCreate an object." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& precision = X.precision();
            LOOP2_ASSERT(D1, precision, D1 == precision);

            const T2& style = X.style();
            LOOP2_ASSERT(D2, style, D2 == style);

            const T3& sign = X.sign();
            LOOP2_ASSERT(D3, sign, D3 == sign);

            const T4& inf = X.infinity();
            LOOP2_ASSERT(D4, inf, strcmp(D4, inf) == 0);

            const T5& nan = X.nan();
            LOOP2_ASSERT(D5, nan, strcmp(D5, nan) == 0);

            const T6& snan = X.sNan();
            LOOP2_ASSERT(D6, snan, strcmp(D6, snan) == 0);

            const T7& point = X.decimalPoint();
            LOOP2_ASSERT(D7, point, D7 == point);

            const T8& exponent = X.exponent();
            LOOP2_ASSERT(D8, exponent, D8 == exponent);
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(percision) }
        {
            mX.setPrecision(A1);

            const T1& percision = X.precision();
            LOOP2_ASSERT(A1, percision, A1 == percision);
        }

        if (veryVerbose) { T_ Q(style) }
        {
            mX.setStyle(A2);

            const T2& style = X.style();
            LOOP2_ASSERT(A2, style, A2 == style);
        }

        if (veryVerbose) { T_ Q(sign) }
        {
            mX.setSign(A3);

            const T3& sign = X.sign();
            LOOP2_ASSERT(A3, sign, A3 == sign);
        }

        if (veryVerbose) { T_ Q(infinity) }
        {
            mX.setInfinity(A4);

            const T4& inf = X.infinity();
            LOOP2_ASSERT(A4, inf, strcmp(A4, inf) == 0);
        }

        if (veryVerbose) { T_ Q(nan) }
        {
            mX.setNan(A5);

            const T5& nan = X.nan();
            LOOP2_ASSERT(A5, nan, strcmp(A5, nan) == 0);
        }

        if (veryVerbose) { T_ Q(snan) }
        {
            mX.setSNan(A6);

            const T6& snan = X.sNan();
            LOOP2_ASSERT(A6, snan, strcmp(A6, snan) == 0);
        }

        if (veryVerbose) { T_ Q(decimalPoint) }
        {
            mX.setDecimalPoint(A7);

            const T7& point = X.decimalPoint();
            LOOP2_ASSERT(D7, point, A7 == point);
        }

        if (veryVerbose) { T_ Q(exponent) }
        {
            mX.setExponent(A8);

            const T8& exponent = X.exponent();
            LOOP2_ASSERT(A8, exponent, A8 == exponent);
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
        //   setPrecision(int value);
        //   setStyle(int value);
        //   setSign(Style value);
        //   setInfinity(bool value);
        //   setNan(bool value);
        //   setSNan(bool value);
        //   setDecimalPoint(int value);
        //   setExponent(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int         D1 = 15;                    // 'precision'
        const Obj::Style  D2 = Obj::e_SCIENTIFIC;     // 'style'
        const Obj::Sign   D3 = Obj::e_NEGATIVE_ONLY;  // 'sign'
        const char       *D4 = "inf";                 // 'infinity'
        const char       *D5 = "nan";                 // 'nan'
        const char       *D6 = "snan";                // 'snan'
        const char        D7 = '.';                   // 'point'
        const char        D8 = 'E';                   // 'exponent
        // 'A' values.

        const int         A1 = 0;                     // 'precision'
        const Obj::Style  A2 = Obj::e_FIXED;          // 'style'
        const Obj::Sign   A3 = Obj::e_ALWAYS;         // 'sign'
        const char       *A4 = "";                    // 'infinity'
        const char       *A5 = "";                    // 'nan'
        const char       *A6 = "";                    // 'snan'
        const char        A7 = ',';                   // 'point'
        const char        A8 = 'e';                   // 'exponent
        // 'B' values.

        const int         B1 = INT_MAX;               // 'precision'
        const Obj::Style  B2 = Obj::e_NATURAL;        // 'style'
        const Obj::Sign   B3 = Obj::e_NEGATIVE_ONLY;  // 'sign'
        const char       *B4 = "Inf";                 // 'infinity'
        const char       *B5 = "NaN";                 // 'nan'
        const char       *B6 = "sNaN";                // 'snan'
        const char        B7 = ' ';                   // 'point'
        const char        B8 = '^';                   // 'exponent

        Obj mX(D1, D2, D3, D4, D5, D6, D7, D8);  const Obj& X = mX;

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // ----------
        // 'precsion'
        // ----------
        {
            mX.setPrecision(A1);
            ASSERT(A1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setPrecision(B1);
            ASSERT(B1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setPrecision(D1);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // -------
        // 'style'
        // -------
        {
            mX.setStyle(A2);
            ASSERT(D1 == X.precision());
            ASSERT(A2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setStyle(B2);
            ASSERT(D1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setStyle(D2);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // ------
        // 'sign'
        // ------
        {
            mX.setSign(A3);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(A3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setSign(B3);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setSign(D3);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // ----------
        // 'infinity'
        // ----------
        {
            mX.setInfinity(A4);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(A4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setInfinity(B4);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setInfinity(D4);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // -----
        // 'nan'
        // -----
        {
            mX.setNan(A5);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setNan(B5);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(B5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setNan(D5);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // ------
        // 'snan'
        // ------
        {
            mX.setSNan(A6);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setSNan(B6);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(B6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setSNan(D6);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // -------
        // 'point'
        // -------
        {
            mX.setDecimalPoint(A7);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setDecimalPoint(B7);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(B7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());

            mX.setDecimalPoint(D7);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        // ----------
        // 'exponent'
        // ----------
        {
            mX.setExponent(A8);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setExponent(B8);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(B8 == X.exponent());

            mX.setExponent(D8);
            ASSERT(D1 == X.precision());
            ASSERT(D2 == X.style());
            ASSERT(D3 == X.sign());
            ASSERT(D4 == X.infinity());
            ASSERT(D5 == X.nan());
            ASSERT(D6 == X.sNan());
            ASSERT(D7 == X.decimalPoint());
            ASSERT(D8 == X.exponent());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------
            mX.setPrecision(A1);
            mX.setStyle(A2);
            mX.setSign(A3);
            mX.setInfinity(A4);
            mX.setNan(A5);
            mX.setSNan(A6);
            mX.setDecimalPoint(A7);
            mX.setExponent(A8);

            ASSERT(A1 == X.precision());
            ASSERT(A2 == X.style());
            ASSERT(A3 == X.sign());
            ASSERT(A4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

              // ---------------------------------------
              // Set all attributes to their 'B' values.
              // ---------------------------------------

            mX.setPrecision(B1);

            ASSERT(B1 == X.precision());
            ASSERT(A2 == X.style());
            ASSERT(A3 == X.sign());
            ASSERT(A4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setStyle(B2);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(A3 == X.sign());
            ASSERT(A4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setSign(B3);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(A4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setInfinity(B4);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(A5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setNan(B5);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(B5 == X.nan());
            ASSERT(A6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setSNan(B6);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(B5 == X.nan());
            ASSERT(B6 == X.sNan());
            ASSERT(A7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setDecimalPoint(B7);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(B5 == X.nan());
            ASSERT(B6 == X.sNan());
            ASSERT(B7 == X.decimalPoint());
            ASSERT(A8 == X.exponent());

            mX.setExponent(B8);

            ASSERT(B1 == X.precision());
            ASSERT(B2 == X.style());
            ASSERT(B3 == X.sign());
            ASSERT(B4 == X.infinity());
            ASSERT(B5 == X.nan());
            ASSERT(B6 == X.sNan());
            ASSERT(B7 == X.decimalPoint());
            ASSERT(B8 == X.exponent());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tPrecision" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setPrecision(-1));
                ASSERT_SAFE_PASS(obj.setPrecision(0));
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
        //   bdldfp::DecimalFormatConfig();
        //   ~bdldfp::DecimalFormatConfig();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int         D1 = 15;                    // 'precision'
        const Obj::Style  D2 = Obj::e_SCIENTIFIC;     // 'style'
        const Obj::Sign   D3 = Obj::e_NEGATIVE_ONLY;  // 'sign'
        const char       *D4 = "inf";                 // 'infinity'
        const char       *D5 = "nan";                 // 'nan'
        const char       *D6 = "snan";                // 'snan'
        const char        D7 = '.';                   // 'point'
        const char        D8 = 'E';                   // 'exponent

        if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout << "Verify the object's attribute values." << endl;

                  // -------------------------------------
                  // Verify the object's attribute values.
                  // -------------------------------------

        LOOP2_ASSERT(D1, X.precision(),    D1 == X.precision());
        LOOP2_ASSERT(D2, X.style(),        D2 == X.style());
        LOOP2_ASSERT(D3, X.sign(),         D3 == X.sign());
        LOOP2_ASSERT(D4, X.infinity(),     strcmp(D4, X.infinity()) == 0);
        LOOP2_ASSERT(D5, X.nan(),          strcmp(D5, X.nan()) == 0);
        LOOP2_ASSERT(D6, X.sNan(),         strcmp(D6, X.sNan()) == 0);
        LOOP2_ASSERT(D7, X.decimalPoint(), D2 == X.style());
        LOOP2_ASSERT(D8, X.exponent(),     D2 == X.style());

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
        //: 1 Create an object 'X'.
        //: 2 Get access to each attribute.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int         PRECISION = 15;
        const Obj::Style  STYLE     = Obj::e_SCIENTIFIC;
        const Obj::Sign   SIGN      = Obj::e_NEGATIVE_ONLY;
        const char       *INF       = "inf";
        const char       *QNAN      = "nan";
        const char       *SNAN      = "snan";
        const char        POINT     = '.';
        const char        EXPONENT  = 'E';

        Obj mX; const Obj& X = mX;

        ASSERT(PRECISION == X.precision());
        ASSERT(STYLE     == X.style());
        ASSERT(SIGN      == X.sign());
        ASSERT(INF       == X.infinity());
        ASSERT(QNAN      == X.nan());
        ASSERT(SNAN      == X.sNan());
        ASSERT(POINT     == X.decimalPoint());
        ASSERT(EXPONENT  == X.exponent());

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
