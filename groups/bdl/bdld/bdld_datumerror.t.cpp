// bdld_datumerror.t.cpp                                              -*-C++-*-
#include <bdld_datumerror.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_string.h>

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
// The component under the test is an attribute class 'bdld::DatumError'.
//
//: o Primary Manipulators:
//:   - DatumError();
//:   - DatumError(int code);
//:   - DatumError(int code, const StringRef& message);
//: o Basic Accessors:
//:   - int code() const;
//:   - StringRef message() const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] DatumError();
// [ 2] DatumError(int code);
// [ 2] DatumError(int code, const StringRef& message);
// [ 4] DatumError(const DatumError&) = default;
// [ 2] ~DatumError() = default;
//
// ACCESSORS
// [ 2] int code() const;
// [ 2] StringRef message() const;
// [ 5] ostream& print(ostream& s, int level, int spacesPerLevel) const;
//
// MANIPULATORS
// [ 4] DatumError& operator=(const DatumError&) = default;
//
// FREE OPERATORS
// [ 3] bool operator==(const DatumError&, const DatumError&);
// [ 3] bool operator!=(const DatumError&, const DatumError&);
// [ 6] bool operator<(const DatumError&, const DatumError&);
// [ 6] bool operator<=(const DatumError&, const DatumError&);
// [ 6] bool operator>(const DatumError&, const DatumError&);
// [ 6] bool operator>=(const DatumError&, const DatumError&);
// [ 5] ostream& operator<<(ostream& stream, const DatumError& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 7] TYPE TRAITS

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

typedef bdld::DatumError Obj;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumError' usage
///- - - - - - - - - - - - - - - - - -
// Suppose we need a function to verify if newly created password meets basic
// security requirements.  Password must contain at least one uppercase letter,
// one lowercase letter, one numeral and one special symbol.  The following
// code illustrates how to use 'bdlb::DatumError' to notify user about
// password weaknesses.
//
// First, we need to write a verification function:
//..
    bdld::DatumError verifyNewPassword(const char *password)
        // Verify if specified 'password' meets basic security requirements.
    {
        bool uppercasePresence     = false;
        bool lowercasePresence     = false;
        bool numeralPresence       = false;
        bool specialSymbolPresence = false;
//..
// Passed string analysis:
//..
        while (*password) {
            if (*password >= 'A' && *password <= 'Z') {
                uppercasePresence = true;
            }
            if (*password >= 'a' && *password <= 'z') {
                lowercasePresence = true;
            }
            if (*password >= '0' && *password <= '9') {
                numeralPresence = true;
            }
            if (*password >= '!' && *password <= '.') {
                specialSymbolPresence = true;
            }
            ++password;
        }
//..
// Result compilation:
//..
        bdld::DatumError result;

        if (!uppercasePresence) {
            result = bdld::DatumError(1, bslstl::StringRef("Uppercase"));
        } else if (!lowercasePresence) {
            result = bdld::DatumError(2, bslstl::StringRef("Lowercase"));
        } else if (!numeralPresence) {
            result = bdld::DatumError(3, bslstl::StringRef("Numeral"));
        } else if (!specialSymbolPresence) {
            result = bdld::DatumError(4, bslstl::StringRef("Special"));
        }

        return result;
    }
//..

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

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
// Next, we need to create password for verification and call our function:
//..
    bdld::DatumError error = verifyNewPassword("Test");
//..
// Then, check the results:
//..
    ASSERT(bdld::DatumError() != error);
    ASSERT(3                  == error.code());
//..
// Finally, we can print the result to the output stream:
//..
    ostringstream out;
    error.print(out);
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   The object is trivially copyable and should have appropriate bsl
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class has the bsl::is_trivially_copyable trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type. (C-1)
        //
        // Testing:
        //   TYPE TRAITS
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TYPE TRAITS" << endl
                          << "===================" << endl;

        ASSERT((bsl::is_trivially_copyable<Obj>::value));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Each relational operator function reports the intended logical
        //:   relationship.
        //:
        //: 2 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., all relational-comparison operators are free functions).
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 4 The relational-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Create 'DatumError' objects with different values.  Verify the
        //:   correctness of operators.  (C-1..3)
        //:
        //: 2 Use the respective addresses of operators to initialize function
        //:   pointers having the appropriate signatures and return types for
        //:   the operators defined in this component.  (C-4)
        //
        // Testing:
        //    bool operator<(const DatumError&, const DatumError&);
        //    bool operator<=(const DatumError&, const DatumError&);
        //    bool operator>(const DatumError&, const DatumError&);
        //    bool operator>=(const DatumError&, const DatumError&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING RELATIONAL-COMPARISON OPERATORS" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "\nTesting operators '>=' and '<'." << endl;
        {
            const Obj obj1(7);
            const Obj obj2(7);
            const Obj obj3(5);

            ASSERT(obj1  >= obj2);
            ASSERT(obj1  >= obj3);
            ASSERT(obj3   < obj1);
            ASSERT(!(obj1 < obj3));

            const Obj obj4(6, "f");
            const Obj obj5(6, "f");
            const Obj obj6(4, "f");
            const Obj obj7(6, "e");

            ASSERT(obj4  >= obj5);
            ASSERT(obj4  >= obj6);
            ASSERT(obj4  >= obj7);
            ASSERT(obj6   < obj4);
            ASSERT(obj7   < obj4);
            ASSERT(!(obj4 < obj6));
            ASSERT(!(obj4 < obj7));
        }

        if (verbose) cout << "\nTesting operators '<=' and '>'." << endl;
        {
            const Obj obj1(5);
            const Obj obj2(5);
            const Obj obj3(7);

            ASSERT(obj1  <= obj2);
            ASSERT(obj1  <= obj3);
            ASSERT(obj3   > obj1);
            ASSERT(!(obj1 > obj3));

            const Obj obj4(4, "e");
            const Obj obj5(4, "e");
            const Obj obj6(6, "e");
            const Obj obj7(4, "f");

            ASSERT(obj4  <= obj5);
            ASSERT(obj4  <= obj6);
            ASSERT(obj4  <= obj7);
            ASSERT(obj6   > obj4);
            ASSERT(obj7   > obj4);
            ASSERT(!(obj4 > obj6));
            ASSERT(!(obj4 > obj7));
        }

        if (verbose) cout << "\nTesting operators format." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorLe   = bdld::operator<;
            operatorPtr operatorGr   = bdld::operator>;

            operatorPtr operatorLeEq = bdld::operator<=;
            operatorPtr operatorGrEq = bdld::operator>=;

            // Quash potential compiler warnings.

            (void)operatorLe;
            (void)operatorGr;
            (void)operatorLeEq;
            (void)operatorGrEq;
        }
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
        //: 1 Create several 'DatumError' objects with different values, put
        //:   their values to the stream / print their values and verify that
        //:   stream contains data in expected format.
        //
        // Testing:
        //    ostream& print(ostream& s, int level, int spacesPerLevel) const;
        //    ostream& operator<<(ostream& stream, const DatumError& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STREAM OUTPUT" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting 'operator<<'." << endl;
        {
            {
                const Obj     obj;
                ostringstream out;

                out << obj;
                ASSERTV(out.str(), "error(0)" == out.str());
            }
            {
                const Obj     obj(5);
                ostringstream out;

                out << obj;
                ASSERTV(out.str(), "error(5)" == out.str());
            }
            {
                const Obj     obj(5, "generic error");
                ostringstream out;

                out << obj;
                ASSERTV(out.str(), "error(5,'generic error')" == out.str());
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            const Obj obj1(5);
            const Obj obj2(5, "generic error");

            if (verbose) cout << "\tTesting single-line format." << endl;
            {
                ostringstream objStr1;
                ostringstream objStr2;

                ostringstream expObjStr1;
                ostringstream expObjStr2;

                expObjStr1 << "error(5)";
                expObjStr2 << "error(5,'generic error')";

                obj1.print(objStr1, 0, -1);
                obj2.print(objStr2, 0, -1);

                ASSERT(expObjStr1.str() == objStr1.str());
                ASSERT(expObjStr2.str() == objStr2.str());
            }

            if (verbose) cout << "\tTesting multi-line format." << endl;
            {
                ostringstream objStr1;
                ostringstream objStr2;

                ostringstream expObjStr1;
                ostringstream expObjStr2;

                // 0 spaces per level.

                expObjStr1 << "error(" << "\n"
                           << "5" << "\n"
                           << ")"<< "\n";

                expObjStr2 << "error(" << "\n"
                           << "5,'generic error'" << "\n"
                           << ")"<< "\n";

                obj1.print(objStr1, 0, 0);
                obj2.print(objStr2, 0, 0);

                ASSERT(expObjStr1.str() == objStr1.str());
                ASSERT(expObjStr2.str() == objStr2.str());

                // Clearing streams.

                expObjStr1.str("");
                expObjStr2.str("");

                objStr1.str("");
                objStr2.str("");

                // 4 spaces per level.

                expObjStr1 << "error(" << "\n"
                           << "    5" << "\n"
                           << ")"<< "\n";

                expObjStr2 << "error(" << "\n"
                           << "    5,'generic error'" << "\n"
                           << ")"<< "\n";

                obj1.print(objStr1, 0, 4);
                obj2.print(objStr2, 0, 4);

                ASSERT(expObjStr1.str() == objStr1.str());
                ASSERT(expObjStr2.str() == objStr2.str());
            }

            if (verbose) cout << "\tTesting level adjustment." << endl;
            {
                ostringstream objStr1;
                ostringstream objStr2;

                ostringstream expObjStr1;
                ostringstream expObjStr2;

                // Level 0.

                expObjStr1 << "error(" << "\n"
                           << " 5" << "\n"
                           << ")"<< "\n";

                expObjStr2 << "error(" << "\n"
                           << " 5,'generic error'" << "\n"
                           << ")"<< "\n";

                obj1.print(objStr1, 0, 1);
                obj2.print(objStr2, 0, 1);

                ASSERT(expObjStr1.str() == objStr1.str());
                ASSERT(expObjStr2.str() == objStr2.str());

                // Clearing streams.

                expObjStr1.str("");
                expObjStr2.str("");

                objStr1.str("");
                objStr2.str("");

                // Level 4.

                expObjStr1 << "    error(" << "\n"
                           << "     5" << "\n"
                           << "    )"<< "\n";

                expObjStr2 << "    error(" << "\n"
                           << "     5,'generic error'" << "\n"
                           << "    )"<< "\n";

                obj1.print(objStr1, 4, 1);
                obj2.print(objStr2, 4, 1);

                ASSERT(expObjStr1.str() == objStr1.str());
                ASSERT(expObjStr2.str() == objStr2.str());
            }
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
        //: 1 Create 'DatumError' object and it's copy.  Verify sameness of two
        //:   objects.  Verify that origin object is unaffected.  (C-1..2)
        //:
        //: 2 Create 'DatumError' object and assign it to another one.  Verify
        //:   sameness of two objects.  Verify that assigned object is
        //:   unaffected.  (C-3..4)
        //:
        //: 3 Assign object to itself.  Verify that object is unaffected.
        //    (C-5)
        //
        // Testing:
        //    DatumError(const DatumError&) = default;
        //    DatumError& operator=(const DatumError&) = default;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "NON-PRIMARY MANIPULATORS" << endl
                          << "========================" << endl;

        if (verbose) cout << "Testing copy-constructor." << endl;
        {
            Obj        mE1(1, "error message");
            const Obj& E1 = mE1;

            {
                Obj        mE2(E1);
                const Obj& E2 = mE2;

                ASSERT(E1 == E2);

                Obj        mE3 = E1;
                const Obj& E3 = mE3;

                ASSERT(E1 == E3);
            }
            ASSERT(1               == E1.code());
            ASSERT("error message" == E1.message());
        }

        if (verbose) cout << "Testing 'operator='." << endl;
        {

            Obj        mE1(2, "error message");
            const Obj& E1 = mE1;

            // Testing self-assignment.

            mE1 = E1;

            ASSERT(2               == E1.code());
            ASSERT("error message" == E1.message());

            // Testing standard usage.
            {
                Obj        mE2;
                const Obj& E2 = mE2;

                mE2 = E1;

                ASSERT(E1 == E2);
            }
            ASSERT(2               == E1.code());
            ASSERT("error message" == E1.message());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Two objects compare equal if and only if their codes and messages
        //:   objects compare equal.
        //:
        //: 2 Comparison is symmetric.
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 4 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Create 'DatumError' objects with different values.  Verify the
        //:   correctness of 'operator==' and 'operator!='.
        //:   (C-1..3)
        //:
        //: 2 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-4)
        //
        // Testing:
        //    bool operator==(const DatumError&, const DatumError&);
        //    bool operator!=(const DatumError&, const DatumError&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING EQUALITY OPERATORS" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nCompare objects with different values."
                          << endl;
        {
            Obj mE1(1);
            Obj mE2(1, "error message");
            Obj mE3(2, "error message");
            Obj mE4(2, "another error");

            ASSERT(mE1   == mE1);
            ASSERT(mE2   == mE2);
            ASSERT(mE3   == mE3);
            ASSERT(mE4   == mE4);

            ASSERT(!(mE1 != mE1));
            ASSERT(mE1   != mE2);
            ASSERT(mE1   != mE3);
            ASSERT(mE1   != mE4);

            ASSERT(mE2   != mE1);
            ASSERT(!(mE2 != mE2));
            ASSERT(mE2   != mE3);
            ASSERT(mE2   != mE4);

            ASSERT(mE3   != mE1);
            ASSERT(mE3   != mE2);
            ASSERT(!(mE3 != mE3));
            ASSERT(mE3   != mE4);

            ASSERT(mE4   != mE1);
            ASSERT(mE4   != mE2);
            ASSERT(mE4   != mE3);
            ASSERT(!(mE4 != mE4));
        }

        if (verbose) cout << "\nCompare objects with identical values."
                          << endl;
        {
            Obj mE1(1, "error message");
            Obj mE2(1, "error message");

            ASSERT(mE1 == mE2);
            ASSERT(!(mE1!=mE2));
        }

        if (verbose) cout << "\nTesting operators format." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdld::operator==;
            operatorPtr operatorNe = bdld::operator!=;

             // Quash potential compiler warnings.

            (void)operatorEq;
            (void)operatorNe;
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
        //: 3 'DatumError' object can be destroyed.
        //
        // Plan:
        //: 1 Create several 'DatumError' objects using the value constructors
        //:   and verify that all data members are initialized correctly.  Let
        //:   objects go out the scope to verify destructor behavior.  (C-1..3)
        //
        // Testing:
        //    DatumError();
        //    DatumError(int code);
        //    DatumError(int code, const StringRef& message);
        //    ~DatumError() = default;
        //    int code() const;
        //    StringRef message() const;
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                         << "========================================" << endl;

        if (verbose) cout << "\nTesting 'DatumError()'." << endl;
        {
            Obj        mE;
            const Obj& E = mE;

            ASSERT(0  == E.code());
            ASSERT("" == E.message());
        }

        if (verbose) cout << "\nTesting 'DatumError(int code)'." << endl;
        {
            Obj        mE(3);
            const Obj& E = mE;

            ASSERT(3  == E.code());
            ASSERT("" == E.message());
        }

        if (verbose)
            cout << "\nTesting 'DatumError(int, const StringRef&)'." << endl;
        {
            Obj        mE(5, "error message");
            const Obj& E = mE;

            ASSERT(5               == E.code());
            ASSERT("error message" == E.message());
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

        if (verbose) cout << "\nCreation test." << endl;

        {
            Obj        mE(0, "error message");
            const Obj& E = mE;

            ASSERT(0 == E.code());
            ASSERT("error message" == E.message());
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
