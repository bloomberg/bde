// bdlt_iso8601utilparseconfiguration.t.cpp                           -*-C++-*-
#include <bdlt_iso8601utilparseconfiguration.h>

#include <bslim_testutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcmp', 'strcmp'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::ends;
using bsl::ostream;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single, unconstrained (value-semantic)
// attribute class.  The Primary Manipulators and Basic Accessors are
// therefore, respectively, the attribute setters and getters, each of which
// follows our standard unconstrained attribute-type naming conventions:
// 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setBasic'
//: o 'setRelaxed'
//
// Basic Accessors:
//: o 'basic'
//: o 'relaxed'
//
// We will therefore follow our standard 10-case approach to testing
// value-semantic types, with the default constructor and primary manipulators
// tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BDEX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] Config();
// [ 6] Config(const Config& original);
// [ 2] ~Config();
//
// MANIPULATORS
// [ 7] Config& operator=(const Config& rhs);
// [ 2] void setBasic(int value);
// [ 2] void setRelaxed(bool value);
//
// ACCESSORS
// [ 3] bool basic() const;
// [ 3] bool relaxed() const;
//
// [ 4] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const Config& lhs, const Config& rhs);
// [ 5] bool operator!=(const Config& lhs, const Config& rhs);
// [ 4] ostream& operator<<(ostream& s, const Config& d);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::Iso8601UtilParseConfiguration Obj;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used by test cases 4, 6, 7, 9, and 11.

struct DefaultDataRow {
    int  d_line;       // source line number
    bool d_basic;
    bool d_relaxed;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE   basic    relaxed
    { L_,    false,   false },
    { L_,    false,   true, },
    { L_,    true,    false },
    { L_,    true,    true  } };

const int NUM_DEFAULT_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;  // eliminate unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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
// Our type, 'Iso8601UtilParseConfiguration', has two boolean attributes,
// 'basic' and 'relaxed'.
//..
        typedef bdlt::Iso8601UtilParseConfiguration Config;
//..
// A default configured object has both attributes being 'false':
//..
        Config c;
        ASSERT(!c.basic());
        ASSERT(!c.relaxed());
//..
// The 'setBasic' sets the 'basic' attribute, leaves the 'relaxed' attribute
// alone:
//..
        Config c2 = c.setBasic();
        ASSERT( c.basic());
        ASSERT(!c.relaxed());
//..
// 'setBasic' and 'setRelaxed' take a boolean argument that defaults to 'true':
//..
        for (int ii = 0; ii < 16; ++ii) {
            const bool basic    = ii & 1;
            const bool relaxed  = ii & 2;
            const bool basicB   = ii & 4;
            const bool relaxedB = ii & 8;

            // 'c' can have any valid state at this point.

            const Config& c3 = c.setBasic(basic);
            ASSERT(&c3 != &c);                // copy, not reference, returned
            const Config& c4 = c.setRelaxed(relaxed);
            ASSERT(&c4 != &c);                // copy, not reference, returned

            ASSERT(c.basic()   == basic);
            ASSERT(c.relaxed() == relaxed);

            c.setRelaxed(relaxedB);
            c.setBasic(basicB);

            ASSERT(c.relaxed() == relaxedB);
            ASSERT(c.basic()   == basicB);

            Config d = c;    // copy 'c' to 'd'

            ASSERT(d.relaxed() == relaxedB);
            ASSERT(d.basic()   == basicB);

            ASSERT(d == c);
            ASSERT(&d != &c);        // 'd' is a copy, not a reference

            d.setBasic();      // defaults to 'true'
            d.setRelaxed();    // defaults to 'true'

            ASSERT(d.basic()   == true);
            ASSERT(d.relaxed() == true);
        }
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
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
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object 'Z' having the value from 'R1', and use the
        //:     copy constructor to create a second 'const' object 'ZZ' from
        //:     'Z'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the default constructor and primary manipulators to
        //:       create a modifiable 'Obj', 'mX', having the value from 'R2'.
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
        //: 4 Repeat steps similar to those described in P-3 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are created to have the value from 'R1'.  For each
        //:   'R1' in the table of P-2:  (C-5)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a modifiable 'Obj', 'mX', having the value from 'R1', and use
        //:     the copy constructor to create a 'const' object 'ZZ' from 'mX'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z' ('mX'), still has the same value as that of
        //:     'ZZ'.  (C-5)
        //
        // Testing:
        //   Config& operator=(const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE    = DATA[ti].d_line;
            const bool IBASIC   = DATA[ti].d_basic;
            const bool IRELAXED = DATA[ti].d_relaxed;

            if (veryVerbose) {
                T_
                P_(ILINE)
                P_(IBASIC)
                P(IRELAXED)
            }

            Obj mZ;  const Obj& Z = mZ;

            mZ.setBasic(IBASIC);
            mZ.setRelaxed(IRELAXED);

            const Obj ZZ(Z);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ILINE, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  JLINE    = DATA[tj].d_line;
                const bool JBASIC   = DATA[tj].d_basic;
                const bool JRELAXED = DATA[tj].d_relaxed;

                if (veryVerbose) {
                    T_
                    P_(JLINE)
                    P_(JBASIC)
                    P(JRELAXED)
                }

                Obj mX;  const Obj& X = mX;

                mX.setBasic(JBASIC);
                mX.setRelaxed(JRELAXED);

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                ASSERTV(ILINE, JLINE, Z, X, (Z == X) == (ILINE == JLINE));

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, JLINE,  Z,   X,  Z == X);
                ASSERTV(ILINE, JLINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, JLINE, ZZ,   Z, ZZ == Z);
            }

            // self-assignment

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  JLINE    = DATA[tj].d_line;
                const bool JBASIC   = DATA[tj].d_basic;
                const bool JRELAXED = DATA[tj].d_relaxed;

                if (veryVerbose) {
                    T_
                    P_(JLINE)
                    P_(JBASIC)
                    P(JRELAXED)
                }

                Obj mX;  const Obj& X = mX;

                mX.setBasic(JBASIC);
                mX.setRelaxed(JRELAXED);

                const Obj ZZ(X);

                const Obj& Z = mX;

                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
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
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     two 'const' objects, 'Z' and 'ZZ', both having the value from
        //:     'R'.
        //:
        //:   2 Use the copy constructor to create an object 'X' from 'Z'.
        //:     (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   Config(const Config& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE    = DATA[ti].d_line;
            const bool IBASIC   = DATA[ti].d_basic;
            const bool IRELAXED = DATA[ti].d_relaxed;

            if (veryVerbose) {
                T_
                P_(ILINE)
                P_(IBASIC)
                P(IRELAXED)
            }

            Obj mZ;  const Obj& Z = mZ;

            mZ.setBasic(IBASIC);
            mZ.setRelaxed(IRELAXED);

            Obj mZZ;  const Obj& ZZ = mZZ;

            mZZ.setBasic(IBASIC);
            mZZ.setRelaxed(IRELAXED);

            if (veryVerbose) { T_ T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ILINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            // Verify the value of the object.

            ASSERTV(ILINE, Z,  X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(ILINE, ZZ, Z, ZZ == Z);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
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
        //:10 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-8..10)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..7)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-3..4)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Use the default constructor and primary manipulators to
        //:       create a 'const' object, 'X', having the value from 'R1', and
        //:       a second 'const' object, 'Y', having the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1..2, 5..7)
        //
        // Testing:
        //   bool operator==(const Config& lhs, const Config& rhs);
        //   bool operator!=(const Config& lhs, const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(Obj, Obj);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdlt::operator==;
            operatorPtr operatorNe = bdlt::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE    = DATA[ti].d_line;
            const bool IBASIC   = DATA[ti].d_basic;
            const bool IRELAXED = DATA[ti].d_relaxed;

            if (veryVerbose) {
                T_
                P_(ILINE)
                P_(IBASIC)
                P(IRELAXED)
            }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;

                mW.setBasic(IBASIC);
                mW.setRelaxed(IRELAXED);

                ASSERTV(ILINE, W,   W == W);
                ASSERTV(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(ILINE, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            Obj mX;  const Obj& X = mX;

            mX.setBasic(IBASIC);
            mX.setRelaxed(IRELAXED);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  JLINE    = DATA[tj].d_line;
                const bool JBASIC   = DATA[tj].d_basic;
                const bool JRELAXED = DATA[tj].d_relaxed;

                if (veryVerbose) {
                    T_
                    P_(JLINE)
                    P_(JBASIC)
                    P(JRELAXED)
                }

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mY;  const Obj& Y = mY;

                mY.setBasic(JBASIC);
                mY.setRelaxed(JRELAXED);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against basic accessor comparison.

                if (EXP) {
                    ASSERTV(ILINE, JLINE, X, Y,
                         X.basic() == Y.basic() && X.relaxed() == Y.relaxed());
                }
                else {
                    ASSERTV(ILINE, JLINE, X, Y,
                         X.basic() != Y.basic() || X.relaxed() != Y.relaxed());
                }

                // Verify value and commutativity.

                ASSERTV(ILINE, JLINE, X, Y,  EXP == (X == Y));
                ASSERTV(ILINE, JLINE, Y, X,  EXP == (Y == X));

                ASSERTV(ILINE, JLINE, X, Y, !EXP == (X != Y));
                ASSERTV(ILINE, JLINE, Y, X, !EXP == (Y != X));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
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
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& s, const Config& d);
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_formatString_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL
        //---- - ---

        { L_,  0,  0,
                                          "["                                NL
                                          "basic = %s"                       NL
                                          "relaxed = %s"                     NL
                                          "]"                                NL
                                                                             },

        { L_,  0,  1,
                                         "["                                 NL
                                         " basic = %s"                       NL
                                         " relaxed = %s"                     NL
                                         "]"                                 NL
                                                                             },

        { L_,  0, -1,
                                          "["                                SP
                                          "basic = %s"                       SP
                                          "relaxed = %s"                     SP
                                          "]"
                                                                             },

        { L_,  0, -8,
                                      "["                                    NL
                                      "    basic = %s"                       NL
                                      "    relaxed = %s"                     NL
                                      "]"                                    NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL
        //---- - ---

        { L_,  3,  0,
                                          "["                                NL
                                          "basic = %s"                       NL
                                          "relaxed = %s"                     NL
                                          "]"                                NL
                                                                             },

        { L_,  3,  2,
                                  "      ["                                  NL
                                  "        basic = %s"                       NL
                                  "        relaxed = %s"                     NL
                                  "      ]"                                  NL
                                                                             },

        { L_,  3, -2,
                                          "      ["                          SP
                                          "basic = %s"                       SP
                                          "relaxed = %s"                     SP
                                          "]"
                                                                             },

        { L_,  3, -8,
                          "            ["                                    NL
                          "                basic = %s"                       NL
                          "                relaxed = %s"                     NL
                          "            ]"                                    NL
                                                                             },

        { L_, -3,  0,
                                          "["                                NL
                                          "basic = %s"                       NL
                                          "relaxed = %s"                     NL
                                          "]"                                NL
                                                                             },

        { L_, -3,  2,
                                  "["                                        NL
                                  "        basic = %s"                       NL
                                  "        relaxed = %s"                     NL
                                  "      ]"                                  NL
                                                                             },

        { L_, -3, -2,
                                        "["                                  SP
                                        "basic = %s"                         SP
                                        "relaxed = %s"                       SP
                                        "]"
                                                                             },
        { L_, -3, -8,
                          "["                                                NL
                          "                basic = %s"                       NL
                          "                relaxed = %s"                     NL
                          "            ]"                                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL
        //---- - ---

        { L_,  2,  3,
                                "      ["                                    NL
                                "         basic = %s"                        NL
                                "         relaxed = %s"                      NL
                                "      ]"                                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL
        //---- - ---

        { L_, -8, -8,
                                      "["                                    NL
                                      "    basic = %s"                       NL
                                      "    relaxed = %s"                     NL
                                      "]"                                    NL
                                                                             },
        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL
        //---- - ---

        { L_, -9, -9, "[ %s %s ]" },

        { L_, -9, -9, "[ %s %s ]" },

#undef NL
#undef SP

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < 4 * NUM_DATA; ++ti) {
                const int         tqi     = ti / NUM_DATA;
                const int         tti     = ti % NUM_DATA;
                const int         LINE    = DATA[tti].d_line;
                const int         L       = DATA[tti].d_level;
                const int         SPL     = DATA[tti].d_spacesPerLevel;
                const char *const FORMAT  = DATA[tti].d_formatString_p;
                const bool        BASIC   = tqi & 1;
                const bool        RELAXED = tqi & 2;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(BASIC) P(RELAXED) }

                Obj mX;  const Obj& X = mX;
                mX.setBasic(BASIC);
                mX.setRelaxed(RELAXED);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bsl::ostringstream os(&oa);

                char EXP[1000];
                sprintf(EXP, FORMAT, BASIC ? "true" : "false",
                                                   RELAXED ? "true" : "false");

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the two basic accessors returns the value of the
        //:   corresponding attribute of the object.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object, 'X', having the value from 'R'.
        //:
        //:   2 Verify that each basic accessor, invoked on 'X', returns the
        //:     expected value.  (C-1..2)
        //
        // Testing:
        //   bool basic() const;
        //   bool relaxed() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE    = DATA[ti].d_line;
            const bool IBASIC   = DATA[ti].d_basic;
            const bool IRELAXED = DATA[ti].d_relaxed;

            if (veryVerbose) {
                T_
                P_(ILINE)
                P_(IBASIC)
                P(IRELAXED)
            }

            Obj mX;  const Obj& X = mX;

            mX.setBasic(IBASIC);
            mX.setRelaxed(IRELAXED);

            ASSERTV(ILINE, IBASIC   == X.basic());
            ASSERTV(ILINE, IRELAXED == X.relaxed());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Each attribute is modifiable independently.
        //:
        //: 3 Each attribute can be set to represent any value.
        //:
        //: 4 Any argument can be 'const'.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create two sets of attribute values for the object: ('D') values
        //:   corresponding to the default-constructed object and ('A') values
        //:   distinct from those corresponding to the default-constructed
        //:   object.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute accessors
        //:   to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's value,
        //:   passing a 'const' argument representing each of the two test
        //:   values, in turn (see P-1), first to 'Ai', then back to 'Di'.
        //:   After each transition, use the (as yet unproven) basic accessors
        //:   to verify that only the intended attribute value changed.
        //:   (C-3..4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by first
        //:   setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to it's corresponding 'D' value
        //:   and verify after each manipulation that only that attribute's
        //:   value changed.  (C-2)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   Iso8601UtilParseConfiguration();
        //   ~Iso8601UtilParseConfiguration();
        //   void setBasic(int value);
        //   void setRelaxed(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        for (int ti = 0; ti < 8; ++ti) {
            const bool IBASIC   = ti & 1;
            const bool IRELAXED = ti & 2;
            const bool IDEFAULT = ti & 3;

            Obj mX;  const Obj& X = mX;

            ASSERT(!X.basic());
            ASSERT(!X.relaxed());

            Obj mX1;    const Obj& X1 = mX1;
            if (IDEFAULT && IBASIC) {
                mX1 = mX.setBasic();
            }
            else {
                mX1 = mX.setBasic(IBASIC);
            }
            ASSERT(X.basic()  == IBASIC);
            ASSERT(X1.basic() == IBASIC);
            ASSERT(!X.relaxed());
            ASSERT(!X1.relaxed());

            if (IDEFAULT && IRELAXED) {
                const Obj& mC = mX1.setRelaxed();
                ASSERT(&mC != &mX1);
                ASSERT(mC.relaxed() == X1.relaxed());
            }
            else {
                const Obj& mC = mX1.setRelaxed(IRELAXED);
                ASSERT(&mC != &mX1);
                ASSERT(mC.relaxed() == X1.relaxed());
            }
            Obj mX2 = mX1;    const Obj& X2 = mX2;
            ASSERT(X.basic()    == IBASIC);
            ASSERT(X1.basic()   == IBASIC);
            ASSERT(X2.basic()   == IBASIC);
            ASSERT(X1.relaxed() == IRELAXED);
            ASSERT(X2.relaxed() == IRELAXED);

            Obj mY;    const Obj& Y = mY;

            ASSERT(!Y.basic());
            ASSERT(!Y.relaxed());

            Obj mY1 = mY.setRelaxed(IRELAXED);    const Obj& Y1 = mY1;
            ASSERT(Y.relaxed()  == IRELAXED);
            ASSERT(Y1.relaxed() == IRELAXED);
            ASSERT(!Y.basic());
            ASSERT(!Y1.basic());

            Obj mY2 = mY1.setBasic(IBASIC);    const Obj& Y2 = mY2;
            ASSERT(Y.relaxed()  == IRELAXED);
            ASSERT(Y1.relaxed() == IRELAXED);
            ASSERT(Y2.relaxed() == IRELAXED);
            ASSERT(Y1.basic()   == IBASIC);
            ASSERT(Y2.basic()   == IBASIC);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        Obj mX;    const Obj& X = mX;

        ASSERT(!X.basic());
        ASSERT(!X.relaxed());

        mX.setBasic();

        ASSERT( X.basic());
        ASSERT(!X.relaxed());

        mX.setBasic(false);

        ASSERT(!X.basic());
        ASSERT(!X.relaxed());

        mX.setRelaxed();

        ASSERT(!X.basic());
        ASSERT( X.relaxed());

        ASSERT( Obj().setBasic().basic());
        ASSERT(!Obj().setBasic().relaxed());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
