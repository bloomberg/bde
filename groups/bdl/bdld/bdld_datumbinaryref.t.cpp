// bdld_datumbinaryref.t.cpp                                          -*-C++-*-
#include <bdld_datumbinaryref.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>        // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bslim_testutil.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>


using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under the test is an attribute class 'bdld::DatumBinaryRef'.
//
// Create a 'DatumBinaryRef' object and verify that values were correctly
// passed down to the 'd_data_p' and 'd_size' data members.  Also exercise the
// copy construction and assignment operator functionality and verify using the
// equality operator that these objects have the same value.  Verify that all
// comparison operators work as expected.  Verify that streaming operator
// outputs the correctly formatted value.
//
//: o Primary Manipulators:
//:   - DatumBinaryRef();
//:   - DatumBinaryRef(const void* data, int type);
//: o Basic Accessors:
//:   - const void *data() const;
//:   - SizeType size() const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] DatumBinaryRef();
// [ 2] DatumBinaryRef(const void *data, SizeType size);
// [ 4] DatumBinaryRef(const DatumBinaryRef&) = default;
// [ 2] ~DatumBinaryRef() = default;
//
// ACCESSORS
// [ 2] void *data() const;
// [ 2] SizeType size() const;
// [ 5] ostream& print(ostream& s, int level, int spacesPerLevel) const;
//
// MANIPULATORS
// [ 4] DatumBinaryRef& operator=(const DatumBinaryRef&) = default;
//
// FREE OPERATORS
// [ 3] bool operator==(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 3] bool operator!=(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 3] bool operator<(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 3] bool operator<=(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 3] bool operator>(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 3] bool operator>=(const DatumBinaryRef&, const DatumBinaryRef&);
// [ 5] ostream& operator<<(ostream& stream, const DatumBinaryRef& rhs);
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

typedef bdld::DatumBinaryRef  Obj;

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

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumBinaryRef' usage
///- - - - - - - - - - - - - - - - - - - -
// Suppose we have three functions. Data are obtained in the first one (with
// memory allocation), processed in the second one and released (with memory
// deallocation) in the third one.  The following code illustrates how to use
// 'bdlb::DatumBinaryRef' to pass information about memory storage between
// them.
//
// First, we write all three functions:
//..
    bdld::DatumBinaryRef obtainData(size_t size)
        // Allocate array of the specified 'size' and initialize it with some
        // values.
    {
        if (0 == size) {
            return bdld::DatumBinaryRef();                            // RETURN
        }
        int *buffer = new int[size];
        for (size_t i = 0; i < size; ++i) {
            buffer[i] = static_cast<int>(i);
        }
        return bdld::DatumBinaryRef(static_cast<void *>(buffer), size);
    }

    int processData(const bdld::DatumBinaryRef& binaryData)
        // Process data, held by the specified 'binaryData' object.
    {
        ostringstream out;
        binaryData.print(out);

        if (binaryData == bdld::DatumBinaryRef()) {
            return 0;                                                 // RETURN
        }

        int        result = 0;
        const int *array = static_cast<const int *>(binaryData.data());
        for (size_t i = 0; i < binaryData.size(); ++i) {
            result += array[i];
        }
        return result;
    }

    void releaseData(const bdld::DatumBinaryRef& binaryData)
        // Release memory, held by the specified 'binaryData' object.
    {
        const int *array = static_cast<const int *>(binaryData.data());
        delete [] array;
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
// Next, we call first one to obtain data:
//..
    bdld::DatumBinaryRef binaryData = obtainData(5);
//..
// Then we verify the results of second one's call:
//..
    ASSERT(10 == processData(binaryData));
//..
// Finally, we release allocated memory:
//..
    releaseData(binaryData);
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
        //: 1 Create several 'DatumBinaryRef' objects with different values,
        //:   put their values to the stream / print their values and verify
        //:   that stream contains data in expected format.  (C-1)
        //
        // Testing:
        //   ostream& print(ostream& s, int level, int spacesPerLevel) const;
        //   ostream& operator<<(ostream& stream, const DatumBinaryRef& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STREAM OUTPUT" << endl
                          << "=====================" << endl;

        void * const ADDRESS = reinterpret_cast<void *>(0xDEADBEEF);
        const int    SIZE    = 16;

        if (verbose) cout << "\nTesting 'print'." << endl;
        {

            if (veryVerbose) cout << "\tSingle line, indent = [0, -1]" << endl;
            {
                Obj        mB(ADDRESS, SIZE);
                const Obj& B = mB;

                const string EXPECTED = "[ binary = 0xdeadbeef size = 16 ]";

                ostringstream out;
                B.print(out, 0, -1);

                ASSERTV(out.str(), EXPECTED == out.str());
            }

            if (veryVerbose) cout << "\tSingle line, indent = [1, -4]" << endl;
            {
                Obj        mB(ADDRESS, SIZE);
                const Obj& B = mB;

                const string EXPECTED =
                                       "    [ binary = 0xdeadbeef size = 16 ]";

                ostringstream out;
                B.print(out, 1, -4);

                ASSERTV(out.str(), EXPECTED == out.str());
            }

            if (veryVerbose) cout << "\tMulti line,  indent = [-1, 2]" << endl;
            {
                Obj        mB(ADDRESS, SIZE);
                const Obj& B = mB;

                const string EXPECTED = "[\n"\
                                        "    binary = 0xdeadbeef\n"\
                                        "    size = 16\n"\
                                        "  ]\n";

                ostringstream out;
                B.print(out, -1, 2);

                ASSERTV(out.str(), EXPECTED == out.str());
            }

            if (veryVerbose) cout << "\tMulti line,  indent = [1,  2]" << endl;
            {
                Obj        mB(ADDRESS, SIZE);
                const Obj& B = mB;

                const string EXPECTED = "  [\n"\
                                        "    binary = 0xdeadbeef\n"\
                                        "    size = 16\n"\
                                        "  ]\n";

                ostringstream out;
                B.print(out, 1, 2);

                ASSERTV(out.str(), EXPECTED == out.str());
            }
        }
        if (veryVerbose) cout << "\nTesting 'operator<<'" << endl;
        {
            Obj        mB(ADDRESS, SIZE);
            const Obj& B = mB;

            const string EXPECTED = "[ binary = 0xdeadbeef size = 16 ]";

            ostringstream out;
            out << B;

            ASSERTV(out.str(), EXPECTED == out.str());
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
        //: 1 Create 'DatumBinaryRef' object and it's copy.  Verify sameness of
        //:   two objects.  Verify that origin object is unaffected.  (C-1..2)
        //:
        //: 2 Create 'DatumBinaryRef' object and assign it to another one.
        //:   Verify  sameness of two objects.  Verify that assigned object is
        //:   unaffected.  (C-3..4)
        //:
        //: 3 Assign object to itself.  Verify that object is unaffected.
        //:   (C-5)
        //
        // Testing:
        //   DatumBinaryRef(const DatumBinaryRef&) = default;
        //   DatumBinaryRef& operator=(const DatumBinaryRef&) = default;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "NON-PRIMARY MANIPULATORS" << endl
                          << "========================" << endl;

        char       data = 0;
        const char SIZE = 1;

        if (verbose) cout << "Testing copy-constructor." << endl;
        {
            Obj        mB1(static_cast<void *>(&data), SIZE);
            const Obj& B1 = mB1;

            // Direct initialization.

            Obj        mB2(B1);
            const Obj& B2 = mB2;

            ASSERT(B1 == B2);

            // Copy initialization.

            Obj        mB3 = B1;
            const Obj& B3 = mB3;

            ASSERT(B1 == B3);

            ASSERT(static_cast<void *>(&data) == B1.data());
            ASSERT(SIZE                       == B1.size());
        }

        if (verbose) cout << "Testing 'operator='." << endl;
        {
            Obj        mB1(static_cast<void *>(&data), SIZE);
            const Obj& B1 = mB1;

            Obj        mB2;
            const Obj& B2 = mB2;
            Obj        mB3;
            const Obj& B3 = mB3;

            mB3 = mB2 = B1;

            ASSERT(B1 == B2);
            ASSERT(B1 == B3);

            ASSERT(static_cast<void *>(&data) == B1.data());
            ASSERT(SIZE                       == B1.size());

            // Self-assignment.

            mB1 = mB1;

            ASSERT(static_cast<void *>(&data) == B1.data());
            ASSERT(SIZE                       == B1.size());
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
        //:   'DatumBinaryRef' object pairs , and a flag value indicating their
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
        //   bool operator==(const DatumBinaryRef&, const DatumBinaryRef&);
        //   bool operator!=(const DatumBinaryRef&, const DatumBinaryRef&);
        //   bool operator<(const DatumBinaryRef&, const DatumBinaryRef&);
        //   bool operator>(const DatumBinaryRef&, const DatumBinaryRef&);
        //   bool operator<=(const DatumBinaryRef&, const DatumBinaryRef&);
        //   bool operator>=(const DatumBinaryRef&, const DatumBinaryRef&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING COMPARISON OPERATORS" << endl
                          << "============================" << endl;
        {
            const Obj a1(    "",  0);
            const Obj b1(    "",  0);

            const Obj a2(   "1",  1);
            const Obj b2(   "1",  1);

            const Obj b3(   "2",  1);

            const Obj a4(  "12",  2);
            const Obj b4(  "12",  2);

            const Obj b5(  "21",  2);
            const Obj b6(  "32",  2);

            const struct {
                const int d_line;             // line
                Obj       d_lhs;              // left comparison argument
                Obj       d_rhs;              // right comparison argument
                const int d_expectedCompare;  // expected comparison result
            } DATA [] = {
                // LINE    LHS    RHS   COMPARE
                // ----    ---    ---   ------------
                {  L_,     a1,    b1,   EQ | LE | GE   },
                {  L_,     b1,    a1,   EQ | LE | GE   },
                {  L_,     a2,    b2,   EQ | LE | GE   },
                {  L_,     b2,    a2,   EQ | LE | GE   },
                {  L_,     a4,    b4,   EQ | LE | GE   },
                {  L_,     b4,    a4,   EQ | LE | GE   },
                // Cross product b1..b12
                {  L_,     b1,    b1,   EQ | LE | GE   },  // ""    ""
                {  L_,     b1,    b2,   NE | LT | LE   },  // ""    "1"
                {  L_,     b1,    b3,   NE | LT | LE   },  // ""    "2"
                {  L_,     b1,    b4,   NE | LT | LE   },  // ""    "12"
                {  L_,     b1,    b5,   NE | LT | LE   },  // ""    "21"
                {  L_,     b1,    b6,   NE | LT | LE   },  // ""    "32"

                {  L_,     b2,    b1,   NE | GT | GE   },  // "1"   ""
                {  L_,     b2,    b2,   EQ | LE | GE   },  // "1"   "1"
                {  L_,     b2,    b3,   NE | LT | LE   },  // "1"   "2"
                {  L_,     b2,    b4,   NE | LT | LE   },  // "1"   "12"
                {  L_,     b2,    b5,   NE | LT | LE   },  // "1"   "21"
                {  L_,     b2,    b6,   NE | LT | LE   },  // "1"   "32"

                {  L_,     b3,    b1,   NE | GT | GE   },  // "2"   ""
                {  L_,     b3,    b2,   NE | GT | GE   },  // "2"   "1"
                {  L_,     b3,    b3,   EQ | LE | GE   },  // "2"   "2"
                {  L_,     b3,    b4,   NE | GT | GE   },  // "2"   "12"
                {  L_,     b3,    b5,   NE | LT | LE   },  // "2"   "21"
                {  L_,     b3,    b6,   NE | LT | LE   },  // "2"   "32"

                {  L_,     b4,    b1,   NE | GT | GE   },  // "12"  ""
                {  L_,     b4,    b2,   NE | GT | GE   },  // "12"  "1"
                {  L_,     b4,    b3,   NE | LT | LE   },  // "12"  "2"
                {  L_,     b4,    b4,   EQ | LE | GE   },  // "12"  "12"
                {  L_,     b4,    b5,   NE | LT | LE   },  // "12"  "21"
                {  L_,     b4,    b6,   NE | LT | LE   },  // "12"  "32"

                {  L_,     b5,    b1,   NE | GT | GE   },  // "21"  ""
                {  L_,     b5,    b2,   NE | GT | GE   },  // "21"  "1"
                {  L_,     b5,    b3,   NE | GT | GE   },  // "21"  "2"
                {  L_,     b5,    b4,   NE | GT | GE   },  // "21"  "12"
                {  L_,     b5,    b5,   EQ | LE | GE   },  // "21"  "21"
                {  L_,     b5,    b6,   NE | LT | LE   },  // "21"  "32"

                {  L_,     b6,    b1,   NE | GT | GE   },  // "32"  ""
                {  L_,     b6,    b2,   NE | GT | GE   },  // "32"  "1"
                {  L_,     b6,    b3,   NE | GT | GE   },  // "32"  "2"
                {  L_,     b6,    b4,   NE | GT | GE   },  // "32"  "12"
                {  L_,     b6,    b5,   NE | GT | GE   },  // "32"  "21"
                {  L_,     b6,    b6,   EQ | LE | GE   },  // "32"  "32"

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
        //: 3 'DatumBinaryRef' object can be destroyed.  Destruction doesn't
        //:   affect binary data, object has pointed to.
        //:
        //: 4 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an 'DatumBinaryRef' object using the default constructor
        //:   and verify that all data members are initialized correctly.
        //:   (C-1..2)
        //:
        //: 2 Create an 'DatumBinaryRef' object using the value constructor and
        //:   verify that all data members are initialized correctly.  (C-1..2)
        //:
        //: 3 Let the 'DatumBinaryRef' object go out the scope.  Verify that
        //:   binary data isn't affected.  (C-3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   DatumBinaryRef();
        //   DatumBinaryRef(const void *data, SizeType size);
        //   ~DatumBinaryRef() = default;
        //   void *data() const;
        //   SizeType size() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                         << "========================================" << endl;

        if (verbose) cout << "\nTesting 'DatumBinaryRef()'." << endl;
        {
            Obj        mB;
            const Obj& B = mB;

            ASSERT(static_cast<void *>(0) == B.data());
            ASSERT(0                      == B.size());
        }

        if (verbose)
            cout << "\nTesting 'DatumBinaryRef(void *data, SizeType size)'."
                 << endl;
        {
            char       ch = 123;
            ASSERT(123 == ch);
            {
                Obj        mB(static_cast<void *>(&ch), 1);
                const Obj& B = mB;

                ASSERT(static_cast<void *>(&ch) == B.data());
                ASSERT(1                        == B.size());

                // Let the object go out the scope to call destructor.
             }

            // Verify that memory hasn't been corrupted.

            ASSERT (123 == ch);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char       data = 0;
            const char SIZE = 1;

            ASSERT_SAFE_FAIL(Obj(static_cast<void *>(0),     SIZE));
            ASSERT_SAFE_PASS(Obj(static_cast<void *>(0),     0   ));
            ASSERT_SAFE_PASS(Obj(static_cast<void *>(&data), 0   ));
            ASSERT_SAFE_PASS(Obj(static_cast<void *>(&data), SIZE));
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
            Obj        mB(reinterpret_cast<void *>(0xDEADBEEF), 16);
            const Obj& B = mB;
            ASSERT(reinterpret_cast<void *>(0xDEADBEEF) == B.data());
            ASSERT(16                                   == B.size());
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
