// bdlm_metricdescriptor.t.cpp                                        -*-C++-*-
#include <bdlm_metricdescriptor.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

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
//: o 'setMetricNamespace'
//: o 'setMetricName'
//: o 'setObjectTypeName'
//: o 'setObjectIdentifier'
//
// Basic Accessors:
//: o 'metricNamespace'
//: o 'metricName'
//: o 'objectTypeName'
//: o 'objectIdentifier'
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
// [ 2] MetricDescriptor(bslma::Allocator *basicAllocator = 0);
// [ 7] MetricDescriptor(const MetricDescriptor&, *bA = 0);
// [ 3] MetricDescriptor(mNs, mN, oTN, oI, bA = 0);
// [ 2] ~MetricDescriptor();
//
// MANIPULATORS
// [ 9] MetricDescriptor& operator=(const MetricDescriptor&);
// [ 2] void setMetricNamespace(const bsl::string_view& value);
// [ 2] void setMetricName(const bsl::string_view& value);
// [ 2] void setObjectTypeName(const bsl::string_view& value);
// [ 2] void setObjectIdentifier(const bsl::string_view& value);
//
// ACCESSORS
// [ 4] const bsl::string& metricNamespace() const;
// [ 4] const bsl::string& metricName() const;
// [ 4] const bsl::string& objectTypeName() const;
// [ 4] const bsl::string& objectIdentifier() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const MetricDescriptor& lhs, rhs);
// [ 6] bool operator!=(const MetricDescriptor& lhs, rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] CONCERN: Allocator is propagated to the stored objects.

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

typedef bdlm::MetricDescriptor Obj;

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
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

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
        //   operator=(const MetricDescriptor& rhs);
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

        static const struct {
            int         d_line;  // source line number
            const char *d_mns;   // metric namespace
            const char *d_mn;    // metric name
            const char *d_otn;   // object type name
            const char *d_oi;    // object identifier
        } DATA[] = {
            //LN  MNS  MN   OTN  OI
            //--  ---  ---  ---  ---
            { L_,  "",  "",  "",  "" },
            { L_, " ", " ", " ", " " },
            { L_, "a", " ", " ", " " },
            { L_, "a", "b", " ", " " },
            { L_, " ", "b", " ", " " },
            { L_, " ", "b", "c", " " },
            { L_, " ", " ", "c", " " },
            { L_, " ", " ", "c", "d" },
            { L_, " ", " ", " ", "d" },
            { L_, "a", "b", "c", " " },
            { L_, " ", "b", "c", "d" },
            { L_, "a", "b", "c", "d" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout <<
                         "\nCopy-assign every value into every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1 = DATA[ti].d_line;
            const char *MNS1  = DATA[ti].d_mns;
            const char *MN1   = DATA[ti].d_mn;
            const char *OTN1  = DATA[ti].d_otn;
            const char *OI1   = DATA[ti].d_oi;

            Obj mZ(MNS1, MN1, OTN1, OI1);  const Obj& Z = mZ;

            Obj mZZ(MNS1, MN1, OTN1, OI1);  const Obj& ZZ = mZZ;

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP1_ASSERT(LINE1, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2 = DATA[tj].d_line;
                const char *MNS2  = DATA[tj].d_mns;
                const char *MN2   = DATA[tj].d_mn;
                const char *OTN2  = DATA[tj].d_otn;
                const char *OI2   = DATA[tj].d_oi;

                Obj mX(MNS2, MN2, OTN2, OI2);  const Obj& X = mX;

                LOOP2_ASSERT(LINE1, LINE2, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP2_ASSERT(LINE1, LINE2,  Z == X);
                LOOP2_ASSERT(LINE1, LINE2, mR == &mX);

                LOOP2_ASSERT(LINE1, LINE2, ZZ == Z);
            }

            if (verbose) cout << "Testing self-assignment" << endl;

            {
                Obj mX(MNS1, MN1, OTN1, OI1);

                Obj mZZ(MNS1, MN1, OTN1, OI1);  const Obj& ZZ = mZZ;

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
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       supplying it the 'const' object 'Z' (see P-2.1), configured
        //:       appropriately (see P-2.2) using a distinct test allocator for
        //:       the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-8)
        //
        // Testing:
        //   MetricDescriptor(const MetricDescriptor& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        static const struct {
            int         d_line;  // source line number
            const char *d_mns;   // metric namespace
            const char *d_mn;    // metric name
            const char *d_otn;   // object type name
            const char *d_oi;    // object identifier
        } DATA[] = {
            //LN  MNS  MN   OTN  OI
            //--  ---  ---  ---  ---
            { L_,  "",  "",  "",  "" },
            { L_, " ", " ", " ", " " },
            { L_, "a", " ", " ", " " },
            { L_, "a", "b", " ", " " },
            { L_, " ", "b", " ", " " },
            { L_, " ", "b", "c", " " },
            { L_, " ", " ", "c", " " },
            { L_, " ", " ", "c", "d" },
            { L_, " ", " ", " ", "d" },
            { L_, "a", "b", "c", " " },
            { L_, " ", "b", "c", "d" },
            { L_, "a", "b", "c", "d" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout <<
                        "\nCopy construct an object from every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_line;
            const char *MNS  = DATA[ti].d_mns;
            const char *MN   = DATA[ti].d_mn;
            const char *OTN  = DATA[ti].d_otn;
            const char *OI   = DATA[ti].d_oi;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(MNS, MN, OTN, OI, &scratch);   const Obj& Z = mZ;

            Obj mZZ(MNS, MN, OTN, OI, &scratch);  const Obj& ZZ = mZZ;

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(Z);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(Z, 0);
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(Z, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                  } break;
                }
                LOOP2_ASSERT(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP_ASSERT(LINE, Obj() == *objPtr);
                    firstFlag = false;
                }

                // Verify the value of the object.

                LOOP_ASSERT(LINE, X == Z);

                // Verify that the value of 'Z' has not changed.

                LOOP_ASSERT(LINE, ZZ == Z);

                // Also apply the object's 'allocator' accessor, as well as
                // that of 'Z'.

                LOOP_ASSERT(LINE, &oa == X.metricNamespace().allocator());
                LOOP_ASSERT(LINE, &oa == X.metricName().allocator());
                LOOP_ASSERT(LINE, &oa == X.objectTypeName().allocator());
                LOOP_ASSERT(LINE, &oa == X.objectIdentifier().allocator());

                LOOP_ASSERT(LINE, &scratch == Z.metricNamespace().allocator());
                LOOP_ASSERT(LINE, &scratch == Z.metricName().allocator());
                LOOP_ASSERT(LINE, &scratch == Z.objectTypeName().allocator());
                LOOP_ASSERT(LINE,
                                 &scratch == Z.objectIdentifier().allocator());

                // Verify no allocation from the non-object allocator.

                LOOP_ASSERT(LINE, 0 == noa.numBlocksTotal());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                LOOP_ASSERT(LINE, 0 == fa.numBlocksInUse());
                LOOP_ASSERT(LINE, 0 == da.numBlocksInUse());
                LOOP_ASSERT(LINE, 0 == sa.numBlocksInUse());
            }
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
        //   bool operator==(const MetricDescriptor& lhs, rhs);
        //   bool operator!=(const MetricDescriptor& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdlm;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;  // source line number
            const char *d_mns;   // metric namespace
            const char *d_mn;    // metric name
            const char *d_otn;   // object type name
            const char *d_oi;    // object identifier
        } DATA[] = {
            // The first row of the table below represents an object value
            // consisting of "baseline" attribute values.  Each subsequent row
            // differs (slightly) from the first in exactly one attribute
            // value.

            //LN  MNS  MN   OTN  OI
            //--  ---  ---  ---  ---
            { L_, " ", " ", " ", " " },  // baseline

            { L_, "a", " ", " ", " " },
            { L_, "a", "b", " ", " " },
            { L_, " ", "b", " ", " " },
            { L_, " ", "b", "c", " " },
            { L_, " ", " ", "c", " " },
            { L_, " ", " ", "c", "d" },
            { L_, " ", " ", " ", "d" },
            { L_, "a", "b", "c", " " },
            { L_, " ", "b", "c", "d" },
            { L_, "a", "b", "c", "d" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1 = DATA[ti].d_line;
            const char *MNS1  = DATA[ti].d_mns;
            const char *MN1   = DATA[ti].d_mn;
            const char *OTN1  = DATA[ti].d_otn;
            const char *OI1   = DATA[ti].d_oi;

            if (veryVerbose) { T_ P_(LINE1) P_(MNS1)  P_(MN1)  P_(OTN1)
                                  P(OI1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mX(MNS1, MN1, OTN1, OI1);  const Obj& X = mX;

                LOOP1_ASSERT(LINE1,   X == X);
                LOOP1_ASSERT(LINE1, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2 = DATA[tj].d_line;
                const char *MNS2  = DATA[tj].d_mns;
                const char *MN2   = DATA[tj].d_mn;
                const char *OTN2  = DATA[tj].d_otn;
                const char *OI2   = DATA[tj].d_oi;

                if (veryVerbose) { T_ P_(LINE2) P_(MNS2)  P_(MN2)  P_(OTN2)
                                   P(OI2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX(MNS1, MN1, OTN1, OI1);  const Obj& X = mX;
                Obj mY(MNS2, MN2, OTN2, OI2);  const Obj& Y = mY;

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
        //: 1 Use the value constructor to create objects and verify the
        //:   accessors, invoked on a reference providing non-modifiable access
        //:   to the object, return the correct values.  (C-1,2)
        //
        // Testing:
        //   const bsl::string& metricNamespace() const;
        //   const bsl::string& metricName() const;
        //   const bsl::string& objectTypeName() const;
        //   const bsl::string& objectIdentifier() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        {
            bsl::string mns = "namespace";
            bsl::string mn  = "name";
            bsl::string otn = "object type name";
            bsl::string oi  = "object identifier";

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(mns, mn, otn, oi, &sa);  const Obj& X = mX;

            ASSERT(mns == X.metricNamespace());
            ASSERT(mn  == X.metricName());
            ASSERT(otn == X.objectTypeName());
            ASSERT(oi  == X.objectIdentifier());
        }
        {
            bsl::string mns = "namespace2";
            bsl::string mn  = "name2";
            bsl::string otn = "object type name2";
            bsl::string oi  = "object identifier2";

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(mns, mn, otn, oi, &sa);  const Obj& X = mX;

            ASSERT(mns == X.metricNamespace());
            ASSERT(mn  == X.metricName());
            ASSERT(otn == X.objectTypeName());
            ASSERT(oi  == X.objectIdentifier());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The value constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 Create objects using the value constructor with and without
        //:   passing in an allocator, verify the value for each attribute and
        //:   the allocator used by each of the attributes using the
        //:   (untested) basic accessors.  (C-1)
        //
        // Testing:
        //   MetricDescriptor(mNs, mN, oTN, oI, bA = 0);
        //   CONCERN: Allocator is propagated to the stored objects.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTOR" << endl
                          << "=================" << endl;

        {
            if (verbose) {
                cout << "Test value constructor without allocator." << endl;
            }

            bsl::string mns = "namespace";
            bsl::string mn  = "name";
            bsl::string otn = "object type name";
            bsl::string oi  = "object identifier";

            Obj mX(mns, mn, otn, oi);  const Obj& X = mX;

            ASSERT(&defaultAllocator == X.metricNamespace().allocator());
            ASSERT(&defaultAllocator == X.metricName().allocator());
            ASSERT(&defaultAllocator == X.objectTypeName().allocator());
            ASSERT(&defaultAllocator == X.objectIdentifier().allocator());

            ASSERT(mns == X.metricNamespace());
            ASSERT(mn  == X.metricName());
            ASSERT(otn == X.objectTypeName());
            ASSERT(oi  == X.objectIdentifier());
        }
        {
            if (verbose) {
                cout << "Test value constructor with allocator." << endl;
            }

            bsl::string mns = "namespace2";
            bsl::string mn  = "name2";
            bsl::string otn = "object type name2";
            bsl::string oi  = "object identifier2";

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(mns, mn, otn, oi, &sa);  const Obj& X = mX;

            ASSERT(&sa == X.metricNamespace().allocator());
            ASSERT(&sa == X.metricName().allocator());
            ASSERT(&sa == X.objectTypeName().allocator());
            ASSERT(&sa == X.objectIdentifier().allocator());

            ASSERT(mns == X.metricNamespace());
            ASSERT(mn  == X.metricName());
            ASSERT(otn == X.objectTypeName());
            ASSERT(oi  == X.objectIdentifier());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators:
        //      - 'setMetricNamespace'
        //      - 'setMetricName'
        //      - 'setObjectTypeName'
        //      - 'setObjectIdentifier'
        //   operate as expected.
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 The primary manipulators produce the expected values.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the value of each attribute
        //:   (empty) and the allocator used by each of the attributes using
        //:   the (untested) basic accessors.  (C-1)
        //:
        //: 2 Modify each attribute and verify the change using the (untested)
        //:   basic accessors.  (C-2)
        //
        // Testing:
        //   MetricDescriptor();
        //   ~MetricDescriptor();
        //   void setMetricNamespace(const bsl::string_view& value);
        //   void setMetricName(const bsl::string_view& value);
        //   void setObjectTypeName(const bsl::string_view& value);
        //   void setObjectIdentifier(const bsl::string_view& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const bsl::string empty;

        {
            if (verbose) {
                cout <<
             "Create an object using the default constructor and no allocator."
                     << endl;
            }

            Obj mX;  const Obj& X = mX;

            ASSERT(empty == X.metricNamespace());
            ASSERT(empty == X.metricName());
            ASSERT(empty == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            ASSERT(&defaultAllocator == X.metricNamespace().allocator());
            ASSERT(&defaultAllocator == X.metricName().allocator());
            ASSERT(&defaultAllocator == X.objectTypeName().allocator());
            ASSERT(&defaultAllocator == X.objectIdentifier().allocator());
        }
        {
            if (verbose) {
                cout <<
             "Create an object using the default constructor and an allocator."
                     << endl;
            }

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            Obj mX(&sa);  const Obj& X = mX;

            ASSERT(empty == X.metricNamespace());
            ASSERT(empty == X.metricName());
            ASSERT(empty == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            ASSERT(&sa == X.metricNamespace().allocator());
            ASSERT(&sa == X.metricName().allocator());
            ASSERT(&sa == X.objectTypeName().allocator());
            ASSERT(&sa == X.objectIdentifier().allocator());
        }
        {
            if (verbose) {
                cout << "Test primary manipulators." << endl;
            }

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            Obj mX(&sa);  const Obj& X = mX;

            ASSERT(empty == X.metricNamespace());
            ASSERT(empty == X.metricName());
            ASSERT(empty == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            bsl::string mns = "namespace";
            mX.setMetricNamespace(mns);

            ASSERT(mns   == X.metricNamespace());
            ASSERT(empty == X.metricName());
            ASSERT(empty == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            bsl::string mn = "name";
            mX.setMetricName(mn);

            ASSERT(mns   == X.metricNamespace());
            ASSERT(mn    == X.metricName());
            ASSERT(empty == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            bsl::string otn = "object type name";
            mX.setObjectTypeName(otn);

            ASSERT(mns   == X.metricNamespace());
            ASSERT(mn    == X.metricName());
            ASSERT(otn   == X.objectTypeName());
            ASSERT(empty == X.objectIdentifier());

            bsl::string oi = "object identifier";
            mX.setObjectIdentifier(oi);

            ASSERT(mns   == X.metricNamespace());
            ASSERT(mn    == X.metricName());
            ASSERT(otn   == X.objectTypeName());
            ASSERT(oi    == X.objectIdentifier());
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
        //: 1 Create an object 'X'.
        //: 2 Get access to each attribute.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bsl::string empty;

        Obj mX; const Obj& X = mX;

        ASSERT(empty == X.metricNamespace());
        ASSERT(empty == X.metricName());
        ASSERT(empty == X.objectTypeName());
        ASSERT(empty == X.objectIdentifier());

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test,
                 globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test,
                 defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
