// balm_metricid.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricid.h>

#include <balm_category.h>
#include <balm_metricdescription.h>

#include <bdlf_bind.h>

#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// A 'balm::MetricId' is a simple in-core value semantic class whose value is
// the address of a (non-modifiable) 'balm::MetricDescription'.  The class also
// provides auxiliary accessors for accessing the value of the underlying
// metric description.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] balm::MetricId();
// [ 3] balm::MetricId(const balm::MetricDescription *);
// [ 5] balm::MetricId(const balm::MetricId&);
// [ 2] ~balm::MetricId();
// MANIPULATORS
// [ 6] balm::MetricId& operator=(const balm::MetricId&);
// [ 2] const balm::MetricDescription *&description();
// ACCESSORS
// [ 2] const balm::MetricDescription * const & description() const;
// [ 2] bool isValid() const;
// [ 7] const balm::Category *category() const;
// [ 7] const char *categoryName() const;
// [ 8] const char *metricName() const;
// [ 9] bsl::ostream& print(bsl::ostream&) const;
// FREE OPERATORS
// [ 4] bool operator==(const balm::MetricId&, const balm::MetricId&);
// [ 4] bool operator!=(const balm::MetricId&, const balm::MetricId&);
// [10] bool operator<(const balm::MetricId&, const balm::MetricId&);
// [ 9] bsl::ostream& operator<<(bsl::ostream&, const balm::MetricId&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

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

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::MetricId          Obj;
typedef balm::MetricDescription Desc;
typedef balm::Category          Category;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    Category cat_A("A"); const Category *CAT_A = &cat_A;
    Category cat_B("B"); const Category *CAT_B = &cat_B;
    Category cat_C("C"); const Category *CAT_C = &cat_C;
    Category cat_D("D"); const Category *CAT_D = &cat_D;

    const char *NAME_A = "A";
    const char *NAME_B = "B";
    const char *NAME_C = "C";
    const char *NAME_D = "D";

    const Desc desc_AA(CAT_A, NAME_A); const Desc *DESC_AA = &desc_AA;
    const Desc desc_BA(CAT_B, NAME_A); const Desc *DESC_BA = &desc_BA;
    const Desc desc_CA(CAT_C, NAME_A); const Desc *DESC_CA = &desc_CA;
    const Desc desc_AB(CAT_A, NAME_B); const Desc *DESC_AB = &desc_AB;
    const Desc desc_BB(CAT_B, NAME_B); const Desc *DESC_BB = &desc_BB;
    const Desc desc_CB(CAT_C, NAME_B); const Desc *DESC_CB = &desc_CB;

    // Same description values, different description addresses.
    const Desc desc_AA2(CAT_A, NAME_A); const Desc *DESC_AA2 = &desc_AA2;
    const Desc desc_BA2(CAT_B, NAME_A); const Desc *DESC_BA2 = &desc_BA2;
    const Desc desc_CA2(CAT_C, NAME_A); const Desc *DESC_CA2 = &desc_CA2;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following example demonstrates how to create and use a 'balm::MetricId'
// object.  We start by creating two metric description objects:
//..
    balm::Category category("MyCategory");
    balm::MetricDescription descriptionA(&category, "MetricA");
    balm::MetricDescription descriptionB(&category, "MetricB");
//..
// Now we create three 'balm::MetricId' objects.
//..
    balm::MetricId invalidId;
    balm::MetricId metricIdA(&descriptionA);
    balm::MetricId metricIdB(&descriptionB);
//..
// We can access and verify their properties:
//..
    ASSERT(!invalidId.isValid());
    ASSERT( metricIdA.isValid());
    ASSERT( metricIdB.isValid());

    ASSERT(0             == invalidId.description());
    ASSERT(&descriptionA == metricIdA.description());
    ASSERT(&descriptionB == metricIdB.description());

    ASSERT(invalidId != metricIdA);
    ASSERT(invalidId != metricIdB);
    ASSERT(metricIdA != metricIdB);
//..
// We now verify that copies of a metric id have the same value as the
// original.
//..
    balm::MetricId copyMetricIdA(metricIdA);

    ASSERT(metricIdA == copyMetricIdA);
//..
// Note that two 'balm::MetricId' objects that have different
// 'balm::MetricDescription' object addresses are *not* equal, *even* if the
// descriptions have the same name and category.
//..
    balm::MetricDescription newDescriptionB(&category, "MetricB");
    balm::MetricId          differentIdB(&newDescriptionB);

    ASSERT(0 == bsl::strcmp(differentIdB.metricName(),metricIdB.metricName()));
    ASSERT(differentIdB.category() == metricIdB.category());

    ASSERT(metricIdB != differentIdB);   // The 'balm::MetricDescription'
                                         // object addresses are not equal!
    } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING LESS OPERATOR: 'operator<'
        //
        // Concerns:
        //   The less-than operator properly compares to ids
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   the less than operator to compare the values.  Test the values
        //   match 'description() < description()'.
        //
        // Testing:
        // bool operator<(const balm::MetricId&, const balm::MetricId&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'operator<'" << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {        0 },
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(VALUES[i].d_description); const Obj& U = u;
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(VALUES[j].d_description); const Obj& V = v;

                bool isLess = (VALUES[i].d_description <
                               VALUES[j].d_description);

                ASSERT(isLess == (U < V));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const balm::MetricId&);
        //   ostream& print(ostream&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting output operators" << endl;
        struct {
            const Desc *d_description;
            const char *d_expected;
        } DATA [] = {
            {   DESC_AA, "A.A" },
            {   DESC_BA, "B.A" },
            {   DESC_CA, "C.A" },
            {   DESC_AB, "A.B" },
            {   DESC_BB, "B.B" },
            {   DESC_CB, "C.B" }

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i].d_description); const Obj& MX = mX;

            bsl::ostringstream printBuf, operBuf;
            MX.print(printBuf);
            operBuf << MX;

            bsl::string EXP(DATA[i].d_expected);
            bsl::string printVal(printBuf.str());
            bsl::string operVal(operBuf.str());

            if (veryVerbose) {
                P_(printVal); P_(operVal); P(EXP);
            }
            LOOP_ASSERT(i, EXP == printVal);
            LOOP_ASSERT(i, EXP == operVal);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS: metricName()
        //
        // Concerns:
        //   'metricName()' returns 'description()->metricName()'..
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct a set of objects from the set
        //   and validate that the accessor returns the correct value.
        //
        // Testing:
        //   const char *metricName() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'metricName'." << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(VALUES[i].d_description); const Obj& X = x;

            const char *NAME_X = VALUES[i].d_description->name();
            ASSERT(NAME_X == X.metricName());
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS: category(), categoryName(), and 'metricName()'
        //
        // Concerns:
        //   'category()' returns the 'balm::Category' address of the
        //   'balm::MetricDescription' object underlying a 'balm::MetricId',
        //   and 'categoryName()' returns 'category()->categoryName()'.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct a set of objects from the set
        //   and validate that the accessor returns the correct value.
        //
        // Testing:
        //   const balm::Category *category() const;
        //   const char *categoryName() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'category' and 'categoryName'."
                          << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(VALUES[i].d_description); const Obj& X = x;
            const Category *CAT_X = VALUES[i].d_description->category();

            ASSERT(CAT_X         == X.category());
            ASSERT(CAT_X->name() == X.categoryName());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricId& operator=(const balm::MetricId& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator" << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {        0 },
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj u(VALUES[i].d_description); const Obj& U = u;

                Obj v(VALUES[j].d_description); const Obj& V = v;
                Obj w(v); const Obj& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(VALUES[i].d_description); const Obj& U = u;
            Obj w(U);  const Obj& W = w;   // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricId(const balm::MetricId& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor" << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {        0 },
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj w(VALUES[i].d_description); const Obj& W = w;
            Obj x(VALUES[i].d_description); const Obj& X = x;

            Obj y(X); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        //   This test must verify comparisons are made on the *address* of
        //   the underlying 'description()' (not the value).
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        //   Use different addresses to the same 'balm::MetricDescription'
        //   value to verify comparisons are made by address (rather than by
        //   value).
        //
        // Testing:
        //   bool operator==(const balm::MetricId&, const balm::MetricId&);
        //   bool operator!=(const balm::MetricId&, const balm::MetricId&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality." << endl;
        struct {
            const Desc* d_description;
        } VALUES [] = {
            {        0 },
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            { DESC_AA2 },
            { DESC_BA2 },
            { DESC_CA2 },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(VALUES[i].d_description); const Obj& U = u;
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(VALUES[j].d_description); const Obj& V = v;

                bool isEqual = i == j;
                ASSERT(isEqual  == (U == V));
                ASSERT(!isEqual == (U != V))
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Test the two varieties of constructors
        //
        // Plan:
        //   Verify the default constructor.
        //
        //   Next, for a sequence of independent test values, use the
        //   alternative constructor to create and object with a specific value
        //   and verify the values using a basic accessor.
        //
        // Testing:
        //   balm::MetricId()
        //   balm::MetricId(const balm::MetricDescription *)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting constructors." << endl;

        {
            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            Obj x; const Obj& X = x;
            ASSERT(!X.isValid());
            ASSERT(0 == X.description());
        }
        if (veryVerbose) cout << "\tTesting secondary constructor." << endl;

        struct {
            const Desc* d_description;
        } VALUES [] = {
            {        0 },
            {  DESC_AA },
            {  DESC_BA },
            {  DESC_CA },
            {  DESC_AB },
            {  DESC_BB },
            {  DESC_CB },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj mX(VALUES[i].d_description); const Obj& MX = mX;

            bool isValid = 0 != VALUES[i].d_description;

            mX.description() = VALUES[i].d_description;

            ASSERT(isValid                 == MX.isValid());
            ASSERT(VALUES[i].d_description == MX.description());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        // balm::MetricId();
        // ~balm::MetricId();
        // const balm::MetricDescription *&description();
        // const balm::MetricDescription * const & description() const;
        // bool isValid() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting primary manipulators and accessors (bootstrap)"
                 << endl;

        {
            if (veryVerbose) cout << "\tTest default constructor" << endl;

            Obj mX; const Obj& MX = mX;
            ASSERT(!MX.isValid());
            ASSERT(0 == MX.description());
        }
        {
            if (veryVerbose)
                cout << "\tTesting primary manipulators and accessors "
                     << "(bootstrap)" << endl;

            struct {
                const Desc* d_description;
            } VALUES [] = {
                {        0 },
                {  DESC_AA },
                {  DESC_BA },
                {  DESC_CA },
                {  DESC_AB },
                {  DESC_BB },
                {  DESC_CB },
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mX; const Obj& MX = mX;

                ASSERT(!MX.isValid());
                ASSERT(0 == MX.description());

                bool isValid = 0 != VALUES[i].d_description;

                mX.description() = VALUES[i].d_description;

                ASSERT(isValid                 == MX.isValid());
                ASSERT(VALUES[i].d_description == MX.description());
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1.                       x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1.           x1:A x2:
        //    4. Append the same element value A to x2.     x1:A x2:A
        //    5. Append another element value B to x2.      x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3.                  x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DESC_AA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(X1.isValid());
        ASSERT(DESC_AA == mX1.description());
        ASSERT(DESC_AA == X1.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(X2.isValid());
        ASSERT(DESC_AA == mX2.description());
        ASSERT(DESC_AA == X2.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.description() = DESC_BA;

        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isValid());
        ASSERT(DESC_BA == mX1.description());
        ASSERT(DESC_BA == X1.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(!X3.isValid());
        ASSERT(0 == mX3.description());
        ASSERT(0 == X3.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(!X4.isValid());
        ASSERT(0 == mX4.description());
        ASSERT(0 == X4.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.description() = DESC_CA;

        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(X3.isValid());
        ASSERT(DESC_CA == mX3.description());
        ASSERT(DESC_CA == X3.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(X2.isValid());
        ASSERT(DESC_BA == mX2.description());
        ASSERT(DESC_BA == X2.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(X2.isValid());
        ASSERT(DESC_CA == mX2.description());
        ASSERT(DESC_CA == X2.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isValid());
        ASSERT(DESC_BA == mX1.description());
        ASSERT(DESC_BA == X1.description());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
