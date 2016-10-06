// bsltf_argumenttype.t.cpp                                           -*-C++-*-
#include <bsltf_argumenttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_util.h>             // for usage example only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>  // for usage example only

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a single unconstrained (value-semantic)
// attribute class.
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Objects created with default constructor (having attribute value '-1') have
// special meaning in the context of the test framework.  Such objects denote
// objects having 'valid, but unspecified' state in the context of move
// operations.
//
// Global Concerns:
//: o No memory is every allocated from this component.
//-----------------------------------------------------------------------------
// CREATORS
// [  ] ArgumentType();
// [  ] ArgumentType(int data);
// [  ] ArgumentType(const ArgumentType& original);
// [  ] ArgumentType(bslmf::MovableRef<ArgumentType> original);
// [  ] ~ArgumentType();
//
// MANIPULATORS
// [ 9] ArgumentType& operator=(const ArgumentType& rhs);
// [12] ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs);
//
// ACCESSORS
// [  ] operator int() const;
// [13] MoveState::Enum movedInto() const;
// [13] MoveState::Enum movedFrom() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
// [ *] CONCERN: No memory is ever allocated.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsltf::ArgumentType<1> Obj;

typedef bslmf::MovableRefUtil       MoveUtil;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int d_line;  // source line number
    int d_value;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  VALUE
    //----  --------
    { L_,        -1 },      // Use default constructor for this value
    { L_,         0 },
    { L_,         1 },
    { L_,       512 },
    { L_,       999 },
    { L_,   INT_MAX },
};
const size_t DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Passing Arguments of the Correct Type and Order
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to test a function, 'forwardData', that takes a variable
// number of arguments and forwards them to another function (called
// 'delegateFunction', in this example).  Note, that the example below provides
// separate implementations for compilers that support C++11 standard and those
// that do not.  For clarity, we define 'forwardData' in line, and limit our
// expansion of the variadic template to 2 arguments on platforms that don't
// support variadic templates.
//
// First, we show the signature to the variadic function 'delegateFunction',
// that 'forwardData' (which we wish to test) will forward to:
//..
    void delegateFunction()
    {
    }

    void delegateFunction(ArgumentType<1> arg01)
    {
        (void) arg01;
    }

    void delegateFunction(ArgumentType<1> arg01, ArgumentType<2> arg02)
    {
        (void) arg01;
        (void) arg02;
    }
//..
// Then, we define the forwarding function we intend to test:
//..
    #if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

    template <class... Args>
    inline
    void forwardData(Args&&... arguments) {
        delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }

    #elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

    inline
    void forwardData()
    {
        delegateFunction();
    }

    template <class Args_01>
    inline
    void forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01)  arguments_01)
    {
        delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
    }

    template <class Args_01, class Args_02>
    inline
    void forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01)  arguments_01,
                     BSLS_COMPILERFEATURES_FORWARD_REF(Args_02)  arguments_02)
    {
        delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }

    #else

    // The code below is a workaround for the absence of perfect forwarding in
    // some compilers.
    template <class... Args>
    inline
    void forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
    {
        delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }

    #endif
//..
// Finally, we define a test case for 'forwardData' passing 'ArgumentType' as
// variadic arguments to the 'forwardData' function and verifying that
// compilation succeeds:
//..
    void usageExample()
    {
        forwardData();

        ArgumentType<1> A01(1);
        forwardData(A01);

        ArgumentType<1> A11(13);
        ArgumentType<2> A12(28);
        forwardData(A11, A12);

        // Note that passing arguments in a wrong order will fail to compile:
        // ArgumentType<1> A21(3);
        // ArgumentType<2> A22(82);
        // forwardData(A22, A21);
    }
//..
}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'movedFrom' AND 'movedInto' METHODS
        //   Ensure that move attributes are set to the correct values after
        //   object construction, copy-assignement, and move-assignment
        //   operations.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Manually create and/or assign a value to an object.  Verify that
        //:   the accessors for the 'movedFrom' and 'movedInto' attributes
        //:   invoked on a reference providing non-modifiable access to the
        //:   object return the expected value.  (C-1,2)
        //
        // Testing:
        //   MoveState::Enum movedInto() const;
        //   MoveState::Enum movedFrom() const;
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'movedFrom' AND 'movedInto' METHODS"
                            "\n===========================================\n");

        if (verbose) printf("\nTesting default and value constructor.\n");
        {
            Obj mX; const Obj& X = mX;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
        }

        if (verbose) printf("\nTesting copy constructor.\n");
        {
            Obj mX;    const Obj& X = mX;
            Obj mY(X); const Obj& Y = mY;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;
            Obj mY(X); const Obj& Y = mY;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());
        }

        if (verbose) printf("\nTesting copy-assignment.\n");
        {
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;

            mY = X;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;
            Obj mY;    const Obj& Y = mY;

            mY = X;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;
            Obj mY(2); const Obj& Y = mY;

            mY = X;

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());
        }

        if (verbose) printf("\nTesting move constructor.\n");
        {
            Obj mX;                     const Obj& X = mX;
            Obj mY(MoveUtil::move(mX)); const Obj& Y = mY;

            ASSERT(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERT(MoveState::e_MOVED     == X.movedFrom());
            ASSERT(MoveState::e_MOVED     == Y.movedInto());
            ASSERT(MoveState::e_NOT_MOVED == Y.movedFrom());
        }
        {
            Obj mX(1);                  const Obj& X = mX;
            Obj mY(MoveUtil::move(mX)); const Obj& Y = mY;

            ASSERT(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERT(MoveState::e_MOVED     == X.movedFrom());
            ASSERT(MoveState::e_MOVED     == Y.movedInto());
            ASSERT(MoveState::e_NOT_MOVED == Y.movedFrom());
        }

        if (verbose) printf("\nTesting move-assignment.\n");
        {
            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY;

            mY = MoveUtil::move(mX);

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_MOVED     == X.movedFrom());
            ASSERTV(MoveState::e_MOVED     == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());

            mX = MoveUtil::move(mY);

            ASSERTV(MoveState::e_MOVED     == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_MOVED     == Y.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;
            Obj mY;    const Obj& Y = mY;

            mY = MoveUtil::move(mX);

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_MOVED     == X.movedFrom());
            ASSERTV(MoveState::e_MOVED     == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());

            mX = MoveUtil::move(mY);

            ASSERTV(MoveState::e_MOVED     == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_MOVED     == Y.movedFrom());
        }
        {
            Obj mX(1); const Obj& X = mX;
            Obj mY(2); const Obj& Y = mY;

            mY = MoveUtil::move(mX);

            ASSERTV(MoveState::e_NOT_MOVED == X.movedInto());
            ASSERTV(MoveState::e_MOVED     == X.movedFrom());
            ASSERTV(MoveState::e_MOVED     == Y.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedFrom());

            mX = MoveUtil::move(mY);

            ASSERTV(MoveState::e_MOVED     == X.movedInto());
            ASSERTV(MoveState::e_NOT_MOVED == X.movedFrom());
            ASSERTV(MoveState::e_NOT_MOVED == Y.movedInto());
            ASSERTV(MoveState::e_MOVED     == Y.movedFrom());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
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
        //: 4 The source object has a valid, but unspecified value.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   move-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default/value constructor to create two 'const' 'Obj',
        //:     'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-2:
        //:
        //:   3 For each of the iterations (P-3.2):  (C-1, 3..4)
        //:
        //:     2 Use the default/value constructor to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Move-assign 'mX' from 'Z'.
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1,4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'ZZ'.  (C-1)
        //:
        //:       2 'Z' has valid, but unspecified value.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-2:  (C-5)
        //:
        //:   1 Use the default/value constructor to create a modifiable 'Obj'
        //:     'mX'; also use the default/value constructor to create a
        //:     'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Move-assign 'mX' from 'Z'.  (C-5)
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //
        // Testing:
        //   ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1  = DATA[ti].d_line;
            const int VALUE1 = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE1) P(VALUE1) }

            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2  = DATA[tj].d_line;
                const int VALUE2 = DATA[tj].d_value;

                if (veryVerbose) { T_ T_ P_(LINE2) P(VALUE2) }

                // Create source 'Z' and control 'ZZ' objects.
                Obj *zPtr;
                Obj *zzPtr;
                if (VALUE1 == -1) {
                    zPtr  = new (fa) Obj();
                    zzPtr = new (fa) Obj();
                } else {
                    zPtr  = new (fa) Obj(VALUE1);
                    zzPtr = new (fa) Obj(VALUE1);
                }
                ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

                Obj&       mZ = *zPtr;
                const Obj& Z  = mZ;
                const Obj& ZZ = *zzPtr;

                // Create target 'X' object.
                Obj *objPtr;
                if (VALUE2 == -1) {
                    objPtr = new (fa) Obj();
                } else {
                    objPtr = new (fa) Obj(VALUE2);
                }
                ASSERTV(3 * sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr; const Obj& X = mX;

                // Move-assign the object.
                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));

                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);
                ASSERTV(LINE1, LINE2, Z, -1 == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                // Manipulate source
                fa.deleteObject(zPtr);

                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);

                fa.deleteObject(zzPtr);
                fa.deleteObject(objPtr);
            }

            // self-assignment
            {
                // Create source 'X', alias 'Z', and control 'ZZ' objects.
                Obj *zzPtr;
                Obj *objPtr;
                if (VALUE1 == -1) {
                    zzPtr  = new (fa) Obj();
                    objPtr = new (fa) Obj();
                } else {
                    zzPtr  = new (fa) Obj(VALUE1);
                    objPtr = new (fa) Obj(VALUE1);
                }
                ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

                const Obj& ZZ = *zzPtr;

                Obj&       mX = *objPtr;
                const Obj& X  = mX;
                Obj&       mZ = mX;
                const Obj& Z  = mZ;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));

                ASSERTV(LINE1, ZZ, X, ZZ == X);
                ASSERTV(LINE1, ZZ, Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                fa.deleteObject(zzPtr);
                fa.deleteObject(objPtr);

                ASSERTV(0 == fa.numBytesInUse());
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The newly created object has the same value (using the equality
        //:   operator) as that of the original object before the call.
        //:
        //: 2 All internal representations of a given value can be used to
        //:   create a new object of equivalent value.
        //:
        //: 3 The original object is always left in a valid state.
        //:
        //: 4 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        // Plan:
        //: 1 Specify a set 'S' of object values with substantial and varied
        //:   differences to be used sequentially in the following tests; for
        //:   each entry, create a control object.   (C-2)
        //:
        //: 2 Call the move constructor to create the object.
        //:
        //: 3 For each of the object values (P-1) verify the following:
        //:
        //:   1 Verify the newly created object has the same value as that of
        //:     the original object before the call to the move constructor
        //:     (control value).  (C-1)
        //:
        //:   2 Manipulate the original object (after the move construction) to
        //:     ensure it is in a valid state, destroy it, and then manipulate
        //:     the newly created object to ensure that it is in a valid state.
        //:     (C-3,4)
        //
        // Testing:
        //   ArgumentType(bslmf::MovableRef<ArgumentType> original);
        // --------------------------------------------------------------------
        if (verbose) printf("\nMOVE CONSTRUCTOR"
                            "\n================\n");

        if (veryVerbose)
            printf("\n\tTesting move constructor.\n");

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE   = DATA[ti].d_line;
            const int VALUE  = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE) P(VALUE) }

            // Create source 'Z' and control 'ZZ' objects.
            Obj *zPtr;
            Obj *zzPtr;
            if (VALUE == -1) {
                zPtr  = new (fa) Obj();
                zzPtr = new (fa) Obj();
            } else {
                zPtr  = new (fa) Obj(VALUE);
                zzPtr = new (fa) Obj(VALUE);
            }
            ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

            Obj&       mZ = *zPtr;
            const Obj& Z  = mZ;
            const Obj& ZZ = *zzPtr;

            // Move-construct the object.
            Obj *objPtr = new (fa) Obj(MoveUtil::move(mZ));

            ASSERTV(3 * sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr; const Obj& X = mX;

            // Verify the value of the object.
            ASSERTV(VALUE, X, ZZ, X == ZZ);

            // original object is movedFrom
            ASSERTV(VALUE, MoveState::e_MOVED == Z.movedFrom());

            // new object is movedInto
            ASSERTV(VALUE, MoveState::e_MOVED == X.movedInto());

            fa.deleteObject(zPtr);

            ASSERTV(VALUE, X, ZZ, X == ZZ);

            fa.deleteObject(zzPtr);
            fa.deleteObject(objPtr);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   Ensure that 'ArgumentType' has the necessary trait values to
        //   guarantee its expected behavior.
        //
        // Concerns:
        //: 1 The object does no have the 'bslma::UsesBslmaAllocator' trait.
        //
        // Plan:
        //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1,2)
        //
        // Testing:
        //   CONCERN: The object has the necessary type traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        BSLMF_ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);
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
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-2:  (C-1, 3..4)
        //:
        //:   1 Use the value constructor and to create two 'const' 'Obj', 'Z'
        //:     and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-2:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1, 3..4)
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z'.
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'. (C-5)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //
        // Testing:
        //   ArgumentType& operator=(const ArgumentType& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.
            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) printf("\nTesting operator.\n");

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1  = DATA[ti].d_line;
            const int VALUE1 = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE1) P(VALUE1) }

            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2  = DATA[tj].d_line;
                const int VALUE2 = DATA[tj].d_value;

                if (veryVerbose) { T_ T_ P_(LINE2) P(VALUE2) }

                // Create source 'Z' and control 'ZZ' objects.
                Obj *zPtr;
                Obj *zzPtr;
                if (VALUE1 == -1) {
                    zPtr  = new (fa) Obj();
                    zzPtr = new (fa) Obj();
                } else {
                    zPtr  = new (fa) Obj(VALUE1);
                    zzPtr = new (fa) Obj(VALUE1);
                }
                ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

                const Obj& Z  = *zPtr;;
                const Obj& ZZ = *zzPtr;

                Obj *objPtr;
                if (VALUE2 == -1) {
                    objPtr = new (fa) Obj();
                } else {
                    objPtr = new (fa) Obj(VALUE2);
                }
                ASSERTV(3 * sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr; const Obj& X = mX;

                if (veryVerbose) { T_ P_(LINE2) P(int(X)) }

                ASSERTV(LINE1, LINE2, int(Z), int(X),
                        (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);

                ASSERTV(LINE1, LINE2, Z, X, Z == X);
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                fa.deleteObject(zPtr);

                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);

                fa.deleteObject(zzPtr);
                fa.deleteObject(objPtr);
            }

            ASSERTV(0 == fa.numBytesInUse());

            // self-assignment
            {
                // Create source 'X', alias 'Z', and control 'ZZ' objects.
                Obj *zzPtr;
                Obj *objPtr;
                if (VALUE1 == -1) {
                    zzPtr  = new (fa) Obj();
                    objPtr = new (fa) Obj();
                } else {
                    zzPtr  = new (fa) Obj(VALUE1);
                    objPtr = new (fa) Obj(VALUE1);
                }
                ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

                const Obj& ZZ = *zzPtr;

                Obj&       mX = *objPtr;
                const Obj& X  = mX;
                const Obj& Z  = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                ASSERTV(LINE1, ZZ, X, ZZ == X);
                ASSERTV(LINE1, ZZ, Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                fa.deleteObject(zzPtr);
                fa.deleteObject(objPtr);

                ASSERTV(0 == fa.numBytesInUse());
            }
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
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     the 'const' object 'Z' (see P-2.1). (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:
        //:     1 The newly constructed object, 'X', has the same value as
        //:       that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   ArgumentType(const ArgumentType& original);
        // --------------------------------------------------------------------
        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE) P(VALUE) }

            // Create source 'Z' and control 'ZZ' objects.
            Obj *zPtr;
            Obj *zzPtr;
            if (VALUE == -1) {
                zPtr  = new (fa) Obj();
                zzPtr = new (fa) Obj();
            } else {
                zPtr  = new (fa) Obj(VALUE);
                zzPtr = new (fa) Obj(VALUE);
            }
            ASSERTV(2 * sizeof(Obj) == fa.numBytesInUse());

            const Obj& Z  = *zPtr;
            const Obj& ZZ = *zzPtr;

            ASSERTV(Z, ZZ, Z == ZZ);

            // Copy-construct the object.
            Obj *objPtr = new (fa) Obj(Z);

            ASSERTV(3 * sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr; const Obj& X = mX;

            ASSERTV(X, ZZ, X == ZZ);
            ASSERTV(Z, ZZ, Z == ZZ);

            // Manipulate source object.
            fa.deleteObject(zPtr);

            ASSERTV(X, ZZ, X == ZZ);

            fa.deleteObject(zzPtr);
            fa.deleteObject(objPtr);

            ASSERTV(0 == fa.numBytesInUse());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Objects of the test type are compared by their values [implicitly]
        //   converted to 'int'.
        // --------------------------------------------------------------------
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
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the default/value constructor, create an object having
        //:   default attribute values.  Verify that the accessor
        //:   'operator int' invoked on a reference providing non-modifiable
        //:   access to the object return the expected value.  (C-1,2)
        //
        // Testing:
        //   operator int() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE) P(VALUE) }

            // Create the object.
            Obj *objPtr;
            if (VALUE == -1) {
                objPtr = new (fa) Obj();
            } else {
                objPtr = new (fa) Obj(VALUE);
            }
            ASSERTV(sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr; const Obj& X = mX;

            ASSERTV(VALUE, X, VALUE == int(X));

            fa.deleteObject(objPtr);

            ASSERTV(0 == fa.numBytesInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
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
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table of P-1:
        //:
        //:     1 Use the value constructor to create an object having the
        //:       value 'V'.
        //:
        //:     2 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of the object have their
        //:       expected values.  (C-1)
        //
        // Testing:
        //   ArgumentTypeType(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CONSTRUCTOR"
                            "\n=================\n");

        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_ P_(LINE) P(VALUE) }

            Obj *objPtr;
            if (VALUE == -1) {
                continue;
            } else {
                objPtr = new (fa) Obj(VALUE);
            }
            ASSERTV(sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr; const Obj& X = mX;

            ASSERTV(VALUE, X, VALUE == int(X));

            fa.deleteObject(objPtr);

            ASSERTV(0 == fa.numBytesInUse());
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Obj(0));
            ASSERT_SAFE_PASS(Obj(1));
            ASSERT_SAFE_PASS(Obj(INT_MAX));
            ASSERT_SAFE_FAIL(Obj(-1));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        //   Ensure that we can use the default constructor to create an object
        //   (having the default constructed value).
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //
        // Plan:
        //: 1 Default-construct an object and use the individual (as yet
        //:   unproven) salient attribute accessors to verify the
        //:   default-constructed value.  (C-1)
        //
        // Testing:
        //   ArgumentType();
        //   ~ArgumentType();
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CONSTRUCTOR"
                            "\n===================\n");

        const int D = -1;  // default attribute value

        Obj mX; const Obj& X = mX;
        ASSERTV(X, D == int(X));

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

        {
            Obj X;
            ASSERT(-1 == X);
            Obj Y(0);
            ASSERT(0 == Y);
            Obj Z(1);
            ASSERT(1 == Z);
        }
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
