// bslalg_hashtableanchor.t.cpp                                   -*-C++-*-

#include <bslalg_hashtableanchor.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_hashtablebucket.h>

#include <bslalg_hastrait.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <limits>
#include <cstddef>

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDescription'
//: o 'setDstInEffectFlag'
//: o 'setUtcOffsetInSeconds'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'description'
//: o 'dstInEffectFlag'
//: o 'utcOffsetInSeconds'
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
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CLASS METHODS
// [11] static bool isValidUtcOffsetInSeconds(int value);
//
// CREATORS
// [ 2] baetzo_LocalTimeDescriptor(bslma_Allocator *bA = 0);
// [ 3] baetzo_LocalTimeDescriptor(int o, bool f, const SRef& d, *bA = 0);
// [ 7] baetzo_LocalTimeDescriptor(const baetzo_LTDescriptor& o, *bA = 0);
// [ 2] ~baetzo_LocalTimeDescriptor();
//
// MANIPULATORS
// [ 9] operator=(const baetzo_LocalTimeDescriptor& rhs);
// [ 2] setDescription(const StringRef& value);
// [ 2] setDstInEffectFlag(bool value);
// [ 2] setUtcOffsetInSeconds(int value);
//
// [ 8] void swap(baetzo_LocalTimeDescriptor& other);
//
// ACCESSORS
// [ 4] bslma_Allocator *allocator() const;
// [ 4] const string& description() const;
// [ 4] bool dstInEffectFlag() const;
// [ 4] int utcOffsetInSeconds() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baetzo_LocalTimeDescriptor& lhs, rhs);
// [ 6] bool operator!=(const baetzo_LocalTimeDescriptor& lhs, rhs);
// [ 5] operator<<(ostream& s, const baetzo_LocalTimeDescriptor& d);
//
// FREE FUNCTIONS
// [ 8] void swap(baetzo_LocalTimeDescriptor& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 8] CONCERN: Precondition violations are detected when enabled.
// [10] Reserved for 'bslx' streaming.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------
const size_t SIZE_T_MAX = std::numeric_limits<std::size_t>::max();
const size_t SIZE_T_MIN = std::numeric_limits<std::size_t>::min();

typedef bslalg::HashTableAnchor   Obj;
typedef bslalg::HashTableBucket   Bucket;
typedef bslalg::BidirectionalLink Link;
typedef std::size_t               size_t;

struct DefaultValueRow {
    int          d_line;  // source line number
    Bucket      *d_bucketArrayAddress;
    std::size_t  d_arraySize;
    Link        *d_listRootAddress;
};

static
const DefaultValueRow DEFAULT_VALUES[] =
{
    //LINE     BUCKETARRAYADDRESS  ARRAYSIZE LISTROOTADDRESS
    //----     ------------------  --------- ---------------

    // default (must be first)
    { L_,     (Bucket *)         0,           0, (Link *)           0},

    // 'data'
    { L_,     (Bucket *) 0xd3adb33f, SIZE_T_MIN, (Link *)  0xb33fc45e},
    { L_,     (Bucket *) 0xf33db33f,          1, (Link *)  0xb33fc45e},
    { L_,     (Bucket *) 0x00000000,          2, (Link *)  0xb33fc45e},
    { L_,     (Bucket *) 0xd3adb33f,          3, (Link *)  0xb33fc45e},
    { L_,     (Bucket *) 0xd3adb33f, SIZE_T_MAX, (Link *)  0xb33fc45e},
    { L_,     (Bucket *) 0xd3adb33f,          4, (Link *)  0xd3adb33f},
    { L_,     (Bucket *) 0xd3adb33f,          5, (Link *)  0},
};

const int DEFAULT_NUM_VALUES = sizeof DEFAULT_VALUES / sizeof *DEFAULT_VALUES;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits of This Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 9: {
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
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', having the value of
        //:     'R1'.
        //:
        //:   2 For each row 'R2 in the tree of P-2:  (C-1, 3..4)
        //:
        //:     1 Create a modifiable 'Obj', 'mX', having the the value of
        //:       'R2'.
        //:
        //:     2 Assign 'mX' from 'Z'.  (C-1)
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that:
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 For each node 'N1' in tree of P-2:  (C-3, 5)
        //:
        //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
        //:
        //:   1 Create a 'const' 'Obj', 'ZZ', pointing to 'N1'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:   5 Use the equal-comparison operator to verify that 'mX' has the
        //:     same value as 'ZZ'.  (C-5)
        //
        // Testing:
        //   SimpleTestType& operator=(const SimpleTestType& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1    = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) { 
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            const Obj Z(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2    = VALUES[tj].d_line;
                Bucket *EXP_ADD2 = VALUES[tj].d_bucketArrayAddress;
                size_t EXP_SIZE2 = VALUES[tj].d_arraySize;
                Link *EXP_ROOT2 = VALUES[tj].d_listRootAddress;

                if (veryVerbose) {
                    T_ P_(LINE2) P_(EXP_ADD2) P_(EXP_SIZE2) P(EXP_ROOT2)
                }

                Obj mX(EXP_ADD2, EXP_SIZE2, EXP_ROOT2);

                Obj *mR = &(mX = Z);
                ASSERTV(ti, tj, mR, &mX, mR == &mX);

                ASSERTV(ti, tj, Z.arraySize(), mX.arraySize(), Z == mX);
                ASSERTV(ti, tj, Z.arraySize(), ZZ.arraySize(), Z == ZZ);
            }
        }

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) { 
                T_ P_(LINE) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            const Obj& Z = mX;
            Obj *mR = &(mX = Z);
            ASSERTV(ti, mR, &mX, mR == &mX);

            ASSERTV(ti, Z.arraySize(), mX.arraySize(), Z == mX);
            ASSERTV(ti, Z.arraySize(), ZZ.arraySize(), Z == ZZ);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
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
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', having the value of
        //:     'R1'.
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Verify that the newly constructed object 'X', has the same
        //:     value as that of 'Z'.  (C-1)
        //:
        //:   4 Verify that 'Z' still has the same value as that of 'ZZ'.
        //:     (C-3)
        //
        // Testing:
        //   SimpleTestType(const SimpleTestType& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) { 
                T_ P_(LINE) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            const Obj  Z(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            const Obj X(Z);

            ASSERTV(Z.arraySize(), X.arraySize(), Z == X);
            ASSERTV(Z.arraySize(), ZZ.arraySize(), Z == ZZ);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
        //:   to the same node in the same tree.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //; 9 The equality operator's signature and return type are standard.
        //:
        //:10 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-2..3)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value of 'R1'.  Create
        //:       another object 'Y' having the value of 'R2'.
        //:
        //:     3 Verify the commutativity property and the expected return
        //:       value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");


        if (verbose)
            printf("\nAssign the address of each operator to a variable.\n");
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            // operatorPtr operatorEq = operator==;

            // operatorPtr operatorNe = operator!=;

            // (void)operatorEq;  // quash potential compiler warnings
            // (void)operatorNe;
        }

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1 = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) { 
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT); const Obj& X = mX;

            // Ensure an object compares correctly with itself (alias test).
            ASSERTV(X.arraySize(),   X == X);
            ASSERTV(X.arraySize(), !(X != X));

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2 = VALUES[tj].d_line;
                Bucket *EXP_ADD2  = VALUES[tj].d_bucketArrayAddress;
                size_t EXP_SIZE2 = VALUES[tj].d_arraySize;
                Link *EXP_ROOT2 = VALUES[tj].d_listRootAddress;

                bool EXP = ti == tj;

                if (veryVerbose) { 
                    T_ P_(LINE2) P_(EXP_ADD2) P_(EXP_SIZE2) P(EXP_ROOT2)
                }

                Obj mY(EXP_ADD2, EXP_SIZE2, EXP_ROOT2); const Obj& Y = mY;
                
                // Verify value, commutativity

                ASSERTV(X.bucketArrayAddress(), 
                        Y.bucketArrayAddress(), 
                        EXP,  (EXP == (X == Y)));
                ASSERTV(X.arraySize(), Y.arraySize(), EXP,  (EXP == (X == Y)));
                ASSERTV(X.listRootAddress(), Y.listRootAddress(), EXP,  (EXP == (X == Y)));
                
                ASSERTV(X.bucketArrayAddress(), Y.bucketArrayAddress(), EXP,  (EXP == (Y == X)));
                ASSERTV(X.arraySize(), Y.arraySize(), EXP,  (EXP == (Y == X)));
                ASSERTV(X.listRootAddress(), Y.listRootAddress(), EXP,  (EXP == (Y == X)));

                ASSERTV(X.arraySize(), Y.arraySize(), EXP, (!EXP == (X != Y)));
                ASSERTV(X.arraySize(), Y.arraySize(), EXP, (!EXP == (Y != X)));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:    of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the default constructor, create an object having default
        //:   attribute values.  Verify that the accessor for the 'data'
        //:   attribute invoked on a reference providing non-modifiable access
        //:   to the object return the expected value.  (C-1)
        //:
        //: 2 Set the 'data' attribute of the object to another value.  Verify
        //:   that the accessor for the 'data' attribute invoked on a reference
        //:   providing non-modifiable access to the object return the expected
        //:   value.  (C-1, 2)
        //
        // Testing:
        //   int arraySize() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS"
                   "\n===============\n");

        Obj mX; const Obj& X = mX;
        ASSERTV(X.arraySize(), 0 == X.arraySize());

        mX.setArraySize(1);
        ASSERTV(X.arraySize(), 1 == X.arraySize());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the documented constraints.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1)
        //:
        //:   1 Use the value constructor to create an object 'X', having the
        //:     value of 'R1'.
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     the attributes of the object have their expected value.  (C-1)
        //
        // Testing:
        //   SimpleTestType(int data);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1    = VALUES[ti].d_line;
            Bucket   *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t    EXP_SIZE = VALUES[ti].d_arraySize;
            Link     *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) { 
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT); const Obj& X = mX;

            ASSERTV(EXP_ADD  == X.bucketArrayAddress());
            ASSERTV(EXP_SIZE == X.arraySize());
            ASSERTV(EXP_ROOT == X.listRootAddress());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & PRIMARY MANIPULATORS
        //   Ensure that we can use the default constructor to create an object
        //   (having the default constructed value).  Also ensure that we can
        //   use the primary manipulators to put that object into any state
        //   relevant for thorough testing.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three attribute values for the 'data' atrribute 'D', 'A',
        //:   and 'B'.  'D' should be the default value.  'A' and 'B' should be
        //:   the the boundary values.
        //:
        //: 2 Default-construct an object and use the individual (as yet
        //:   unproven) salient attribute accessors to verify the
        //:   default-constructed value.  (C-1)
        //:
        //: 3 Set and object's 'data' attribute to 'A' and 'B'.  Verify the
        //:   state of object using the (as yet unproven) salient attriubte
        //:   accessors.  (C-2)
        //
        // Testing:
        //   SimpleTestType();
        //   ~SimpleTestType();
        //   void setArraySize(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                            "\n===================================\n");

        const size_t D = 0;
        const size_t A = SIZE_T_MAX;
        const size_t B = SIZE_T_MIN;

        Obj mX; const Obj& X = mX;
        ASSERTV(X.arraySize(), D == X.arraySize());

        mX.setArraySize(A);
        ASSERTV(X.arraySize(), A == X.arraySize());

        mX.setArraySize(B);
        ASSERTV(X.arraySize(), B == X.arraySize());



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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj X;
        ASSERT(X.arraySize() == 0);

        X.setArraySize(1);
        ASSERT(X.arraySize() == 1);

        Obj Y(0, 2, 0);
        ASSERT(Y.arraySize() == 2);

        Obj Z(Y);
        ASSERT(Z == Y);
        ASSERT(X != Y);

        X = Z;
        ASSERT(Z == Y);
        ASSERT(X == Y);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
