// bdld_datumudt.t.cpp                                                -*-C++-*-
#include <bdld_datumudt.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_string.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>                  // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under the test is an attribute class 'bdld::DatumUdt'.
//
// Create a 'DatumUdt' object and verify that values were correctly passed down
// to the 'd_data_p' and 'd_type' data members.  Also exercise the copy
// construction and assignment operator functionality and verify using the
// equality operator that these objects have the same value.  Verify that all
// comparison operators work as expected.  Verify that streaming operator
// outputs the correctly formatted value.
//
//: o Primary Manipulators:
//:   - DatumUdt(void* data, int type);
//: o Basic Accessors:
//:   - void *data() const;
//:   - int type() const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] DatumUdt(void *data, int type);
// [ 4] DatumUdt(const DatumUdt&) = default;
// [ 2] ~DatumUdt() = default;
//
// ACCESSORS
// [ 2] void *data() const;
// [ 2] int type() const;
// [ 5] ostream& print(ostream& s, int level, int spacesPerLevel) const;
//
// MANIPULATORS
// [ 4] DatumUdt& operator=(const DatumUdt&) = default;
//
// FREE OPERATORS
// [ 3] bool operator==(const DatumUdt&, const DatumUdt&);
// [ 3] bool operator!=(const DatumUdt&, const DatumUdt&);
// [ 3] bool operator<(const DatumUdt&, const DatumUdt&);
// [ 3] bool operator<=(const DatumUdt&, const DatumUdt&);
// [ 3] bool operator>(const DatumUdt&, const DatumUdt&);
// [ 3] bool operator>=(const DatumUdt&, const DatumUdt&);
// [ 5] ostream& operator<<(ostream& stream, const DatumUdt& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 6] TYPE TRAITS

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

typedef bdld::DatumUdt  Obj;

enum CompareResult {
    // This enumeration is used to describe expected result of compare
    // operators in the tests.
    EQ = 1 << 0,    // Compare equal
    NE = 1 << 1,    // Compare not equal
    LT = 1 << 2,    // Compare less
    LE = 1 << 3,    // Compare less or equal
    GE = 1 << 4,    // Compare greater
    GT = 1 << 5     // Compare greater or equal
} ;

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

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
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
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumUdt' usage
///- - - - - - - - - - - - - - - - -
// Imagine we are using 'Datum' within an expression evaluation subsystem.
// Within that subsystem, along with the set of types defined by
// 'Datum::DataType' we also need to hold 'Sequence' and 'Choice' types within
// 'Datum' values (which are not natively represented by 'Datum').  First, we
// define the set of types used by our subsystem that are an extension to the
// types in 'DatumType':
//..
    struct Sequence {
        struct Sequence *d_next_p;
        int              d_value;
    };

    enum ExtraExpressionTypes {
        e_SEQUENCE = 5,
        e_CHOICE = 6
    };
//..
// Notice that the numeric values will be provided as the 'type' attribute to
// 'DatumUdt'.
//
// Then we create a 'Sequence' object, and create a 'DatumUdt' to hold it (note
// that we've created the object on the stack for clarity):
//..
    Sequence sequence;
    {
        const bdld::DatumUdt udt(&sequence, e_SEQUENCE);
        ASSERT(e_SEQUENCE == udt.type());
        ASSERT(&sequence  == udt.data());
    }
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   The object is trivially copyable and should have appropriate bsl
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class has the bsl::is_trivially_copyable trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type.  (C-1)
        //
        // Testing:
        //   TYPE TRAITS
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TYPE TRAITS" << endl
                          << "===================" << endl;

        ASSERT((bsl::is_trivially_copyable<Obj>::value));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING STREAM OUTPUT
        //
        // Concerns:
        //: 1 Stream output operator and 'print' method put object's value to
        //:   the specified stream in expected format.
        //
        // Plan:
        //: 1 Create several 'DatumUdt' objects with different values, put
        //:   their values to the stream / print their values and verify that
        //:   stream contains data in expected format.  (C-1)
        //
        // Testing:
        //   ostream& print(ostream& s, int level, int spacesPerLevel) const;
        //   ostream& operator<<(ostream& stream, const DatumUdt& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STREAM OUTPUT" << endl
                          << "=====================" << endl;

        int          udt     = 0xdeadbeef;
        void * const ADDRESS = &udt;
        const int    TYPE    = 15;

        if (verbose) cout << "\nTesting 'print'." << endl;
        {

            if (veryVerbose) cout << "\tSingle line, indent = [0, -1]" << endl;
            {
                Obj        mU(ADDRESS, TYPE);
                const Obj& U = mU;

                ostringstream exp;
                exp << "user-defined(" << ADDRESS << "," << TYPE << ")";

                ostringstream out;
                U.print(out, 0, -1);

                ASSERTV(out.str(), exp.str() == out.str());
            }

            if (veryVerbose) cout << "\tSingle line, indent = [1, -4]" << endl;
            {
                Obj        mU(ADDRESS, TYPE);
                const Obj& U = mU;

                ostringstream exp;
                exp << "    user-defined(" << ADDRESS << "," << TYPE << ")";

                ostringstream out;
                U.print(out, 1, -4);

                ASSERTV(out.str(), exp.str() == out.str());
            }

            if (veryVerbose) cout << "\tMulti line,  indent = [-1, 2]" << endl;
            {
                Obj        mU(ADDRESS, TYPE);
                const Obj& U = mU;

                ostringstream exp;
                exp << "user-defined(\n"
                    << "    " << ADDRESS << "\n"
                    << "    " << TYPE << "\n"
                    << "  )\n";

                ostringstream out;
                U.print(out, -1, 2);

                ASSERTV(out.str(), exp.str() == out.str());
            }

            if (veryVerbose) cout << "\tMulti line,  indent = [1,  2]" << endl;
            {
                Obj        mU(ADDRESS, TYPE);
                const Obj& U = mU;

                ostringstream exp;
                exp << "  user-defined(\n"
                    << "    " << ADDRESS << "\n"
                    << "    " << TYPE << "\n"
                    << "  )\n";

                ostringstream out;
                U.print(out, 1, 2);

                ASSERTV(out.str(), exp.str() == out.str());
            }
        }
        if (veryVerbose) cout << "\nTesting 'operator<<'" << endl;
        {
            Obj        mU(ADDRESS, TYPE);
            const Obj& U = mU;

            ostringstream exp;
            exp << "user-defined(" << ADDRESS << "," << TYPE << ")";

            ostringstream out;
            out << U;

            ASSERTV(out.str(), exp.str() == out.str());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // NON-PRIMARY MANIPULATORS
        //   This test will verify that compiler generated copy constructor and
        //   assignment work as expected.
        //
        // Concerns:
        //: 1 The copy-constructor sets the same value as has original object
        //:   to the newly created one.
        //:
        //: 2 The copy-constructor leaves the value of the original object
        //:   unaffected.
        //:
        //: 3 The value represented by any instance can be assigned to any
        //:   other instance.
        //:
        //: 4 The 'rhs' value must not be affected by the assignment operation.
        //:
        //: 5 Aliasing (x = x): The assignment operator must always work --
        //:   even when the lhs and rhs are the same object.
        //
        // Plan:
        //: 1 Create 'DatumUdt' object and it's copy.  Verify sameness of two
        //:   objects.  Verify that origin object is unaffected.  (C-1..2)
        //:
        //: 2 Create 'DatumUdt' object and assign it to another one.  Verify
        //:   sameness of two objects.  Verify that assigned object is
        //:   unaffected.  (C-3..4)
        //:
        //: 3 Assign object to itself.  Verify that object is unaffected.
        //:   (C-5)
        //
        // Testing:
        //   DatumUdt(const DatumUdt&) = default;
        //   DatumUdt& operator=(const DatumUdt&) = default;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "NON-PRIMARY MANIPULATORS" << endl
                          << "========================" << endl;

        if (verbose) cout << "Testing copy-constructor." << endl;
        {
            int       udt  = 0xdeadbeef;
            const int type = 15;

            Obj        mU1(&udt, type);
            const Obj& U1 = mU1;

            ASSERT(&udt == U1.data());
            ASSERT(type == U1.type());

            // Direct initialization.

            Obj        mU2(U1);
            const Obj& U2 = mU2;

            ASSERT(U1 == U2);

            // Copy initialization.

            Obj        mU3 = U1;
            const Obj& U3 = mU3;

            ASSERT(U1 == U3);
        }

        if (verbose) cout << "Testing 'operator='." << endl;
        {
            int       udt  = 0xdeadbeef;
            const int type = 15;

            Obj        mU1(&udt, type);
            const Obj& U1 = mU1;

            Obj        mU2(0, 0);
            const Obj& U2 = mU2;
            Obj        mU3(&udt + 1, type + 1);
            const Obj& U3 = mU3;


            mU3 = mU2 = U1;

            ASSERT(U1 == U2);
            ASSERT(U1 == U3);

           // Self-assignment.

            mU1 = mU1;

            ASSERT(&udt == U1.data());
            ASSERT(type == U1.type());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Two objects compare equal if and only if sizes of their binary
        //:   data and binary data itself are compare equal.
        //:
        //: 2 Each relational operator function reports the intended logical
        //:   relationship.
        //:
        //: 3 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., all relational-comparison operators are free functions).
        //:
        //: 4 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 5 The equality-comparison operators' signatures and return types
        //:   are standard.
        //:
        //: 6 The relational-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of
        //:   'DatumUdt' object pairs , and a flag value indicating their
        //:   relationship.
        //:
        //: 2 For each row 'R' in the table of P-1 verify that each tested
        //:   operator returns the expected value.  (C-1..4)
        //:
        //: 3 Use the respective addresses of operators to initialize function
        //:   pointers having the appropriate signatures and return types for
        //:   the operators defined in this component.  (C-5..6)
        //
        // Testing:
        //   bool operator==(const DatumUdt&, const DatumUdt&);
        //   bool operator!=(const DatumUdt&, const DatumUdt&);
        //   bool operator<(const DatumUdt&, const DatumUdt&);
        //   bool operator>(const DatumUdt&, const DatumUdt&);
        //   bool operator<=(const DatumUdt&, const DatumUdt&);
        //   bool operator>=(const DatumUdt&, const DatumUdt&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING COMPARISON OPERATORS" << endl
                          << "============================" << endl;
        {
            int       udt  = 0xdeadbeef;
            const int type = 15;

            const Obj U1(0,      0);
            const Obj U2(0,      0);
            const Obj U3(&udt,   type);
            const Obj U4(&udt,   type+1);
            const Obj U5(&udt+1, type);

            const struct {
                const int d_line;             // line
                Obj       d_lhs;              // left comparison argument
                Obj       d_rhs;              // right comparison argument
                const int d_expectedCompare;  // expected comparison result
            } DATA [] = {
                // LINE    LHS    RHS   COMPARE
                // ----    ---    ---   -------
                {  L_,     U1,    U1,   EQ | LE | GE   },
                {  L_,     U1,    U2,   EQ | LE | GE   },
                {  L_,     U1,    U3,   NE | LT | LE   },

                {  L_,     U2,    U1,   EQ | LE | GE   },
                {  L_,     U2,    U2,   EQ | LE | GE   },
                {  L_,     U2,    U3,   NE | LT | LE   },
                {  L_,     U2,    U4,   NE | LT | LE   },
                {  L_,     U2,    U5,   NE | LT | LE   },

                {  L_,     U3,    U2,   NE | GT | GE   },
                {  L_,     U3,    U3,   EQ | LE | GE   },
                {  L_,     U3,    U4,   NE | LT | LE   },
                {  L_,     U3,    U5,   NE | LT | LE   },

                {  L_,     U4,    U2,   NE | GT | GE   },
                {  L_,     U4,    U3,   NE | GT | GE   },
                {  L_,     U4,    U4,   EQ | LE | GE   },
                {  L_,     U4,    U5,   NE | LT | LE   },

                {  L_,     U5,    U2,   NE | GT | GE   },
                {  L_,     U5,    U3,   NE | GT | GE   },
                {  L_,     U5,    U4,   NE | GT | GE   },
                {  L_,     U5,    U5,   EQ | LE | GE   },
            };

            const size_t NUM_DATA = sizeof(DATA)/sizeof(DATA[0]);

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int  LINE     = DATA[i].d_line;
                const Obj& LHS      = DATA[i].d_lhs;
                const Obj& RHS      = DATA[i].d_rhs;
                const int  EXPECTED = DATA[i].d_expectedCompare;

                for (int j = 0; j < 6; ++j) {
                    bool result;
                    bool expected;
                    switch (1 << j) {
                      case EQ:
                        result = LHS == RHS;
                        expected = EXPECTED & EQ;
                        break;
                      case NE:
                        result = LHS != RHS;
                        expected = EXPECTED & NE;
                        break;
                      case LT:
                        result = LHS <  RHS;
                        expected = EXPECTED & LT;
                        break;
                      case GT:
                        result = LHS >  RHS;
                        expected = EXPECTED & GT;
                        break;
                      case LE:
                        result = LHS <= RHS;
                        expected = EXPECTED & LE;
                        break;
                      case GE:
                        result = LHS >= RHS;
                        expected = EXPECTED & GE;
                        break;
                      default: ASSERT(false);
                    }

                    ASSERTV(LINE, j, result, result == expected);
                }
            }
        }
        if (verbose) cout << "\nTesting operators format." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq   = bdld::operator==;
            operatorPtr operatorNe   = bdld::operator!=;

            operatorPtr operatorLe   = bdld::operator<;
            operatorPtr operatorGr   = bdld::operator>;

            operatorPtr operatorLeEq = bdld::operator<=;
            operatorPtr operatorGrEq = bdld::operator>=;

             // Quash potential compiler warnings.

            (void)operatorEq;
            (void)operatorNe;
            (void)operatorLe;
            (void)operatorGr;
            (void)operatorLeEq;
            (void)operatorGrEq;
        }

      } break;
      case 2: {
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
        //: 2 Accessors return expected values.
        //:
        //: 3 'DatumUdt' object can be destroyed.  Destruction doesn't
        //:   affect data, object has pointed to.
        //
        // Plan:
        //: 1 Create an 'DatumUdt' value using the value constructors and
        //:   verify that all data members are initialized correctly.  (C-1..2)
        //:
        //: 2 Let the 'DatumUdt' object go out the scope.  Verify that data
        //:   isn't affected.  (C-3)
        //
        // Testing:
        //   DatumUdt(void *data, int type);
        //   ~DatumUdt() = default;
        //   void *data() const;
        //   int type() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                         << "========================================" << endl;

        if (verbose) cout << "\nTesting constructor."
                          << endl;
        {
            unsigned int udt  = 0xfeedbee0;
            const int    type = 15;
            ASSERT(0xfeedbee0 == udt);
            {
                Obj        mU(&udt, type);
                const Obj& U = mU;

                ASSERT(&udt == U.data());
                ASSERT(type == U.type());

                // Let the object go out the scope to call destructor.
            }

            // Verify that memory hasn't been corrupted.

            ASSERT(0xfeedbee0 == udt);
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

        {
            int       udt  = 0xfeedbee0;
            const int type = 15;

            Obj        mU(&udt, type);
            const Obj& U = mU;

            ASSERT(&udt == U.data());
            ASSERT(type == U.type());

            if (veryVerbose) cout << U;
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
