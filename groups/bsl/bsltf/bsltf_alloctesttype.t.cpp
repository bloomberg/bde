// bsltf_alloctesttype.t.cpp                                          -*-C++-*-
#include <bsltf_alloctesttype.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <new>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a single unconstrained (value-semantic)
// attribute class.  The Primary Manipulators and Basic Accessors are
// therefore, respectively, the attribute setters and getters, each of which
// follows our standard unconstrained attribute-type naming conventions:
// 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setData'
//
// Basic Accessors:
//: o 'data'
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] AllocTestType(bslma::Allocator *bA = 0);
// [ 3] AllocTestType(int data);
// [ 7] AllocTestType(const AllocTestType& original);
// [ 2] ~AllocTestType();
//
// MANIPULATORS
// [ 9] AllocTestType& operator=(const AllocTestType& rhs);
// [ 2] void setData(int value);
//
// ACCESSORS
// [ 4] int data() const;
// [ 4] bslma::Allocator *allocator() const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [11] CONCERN: The object has the necessary type traits
// [12] CONCERN: Bitwise-moved objects assert on destruction
// [ *] CONCERN: In no case does memory come from the global allocator.

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

typedef bsltf::AllocTestType Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

struct DefaultValueRow {
    int d_line;  // source line number
    int d_data;
};

static
const DefaultValueRow DEFAULT_VALUES[] =
{
    //LINE     DATA
    //----     ----

    // default (must be first)
    { L_,         0 },

    // 'data'
    { L_,   INT_MIN },
    { L_,        -1 },
    { L_,         1 },
    { L_,   INT_MAX },
};
const int DEFAULT_NUM_VALUES = sizeof DEFAULT_VALUES / sizeof *DEFAULT_VALUES;

// ============================================================================
//                     GLOBAL FUNCTIONS USED FOR TESTING
// ----------------------------------------------------------------------------

static
void exitHandler(const char *, const char *, int)
    // Call 'exit' with the current 'testStatus' of this test driver.  This
    // function is intended to be used as an assertion handler, registered with
    // 'bsls_assert'.  To check that an assertion is triggered (when expected)
    // in a destructor, we need a code path that can exit the process directly
    // without returning control to the test handler.  In C++11, destructors
    // are expected to have 'noexcept(true)' exception specifications, which is
    // the new default, so we cannot rely on the regular assert test handler
    // that indicates an expected assertion by throwing an exception.  Note
    // that the expected effect is 'exit(0)' as 'testStatus' should be zero for
    // a passing test driver.  However, we do not want to lose reporting any
    // other error detected in the current test case, so return the value of
    // 'testStatus'.
{
    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    exit(testStatus);
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits of This Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template 'printTypeTraits' with a parameterized
// 'TYPE':
//..
template <class TYPE>
void printTypeTraits()
    // Prints the traits of the parameterized 'TYPE' to the console.
{
    if (bslma::UsesBslmaAllocator<TYPE>::value) {
        printf("Type defines bslma::UsesBslmaAllocator.\n");
    }
    else {
        printf("Type does not define bslma::UsesBslmaAllocator.\n");
    }

    if (bslmf::IsBitwiseMoveable<TYPE>::value) {
        printf("Type defines bslmf::IsBitwiseMoveable.\n");
    }
    else {
        printf("Type does not define bslmf::IsBitwiseMoveable.\n");
    }
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
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

// Now, we invoke the 'printTypeTraits' function template using 'AllocTestType'
// as the parameterized 'TYPE':
//..
          printTypeTraits<AllocTestType>();
//..
// Finally, we observe the console output:
//..
//  Type defines bslma::UsesBslmaAllocator.
//  Type does not define bslmf::IsBitwiseMoveable.
//..
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ASSERTION ON BITWISE MOVE
        //   The 'AllocTestType' object is specifically designed to NOT be
        //   bitwise-moveable, so that it can be used to verify code is
        //   correctly testing the 'bslmf::IsBitwiseMoveable' trait before
        //   using such a technique.  The destructor for this class is mined
        //   with a check that the current value of 'this' is the same as when
        //   the object was first created.  If this test fails, an ASSERT_OPT
        //   is triggered.  In C++03 we might test for this using the familiar
        //   'bsls_asserttest' facility, but in C++11 destructors become
        //   implicitly 'noexcept', and throwing a testing exception from the
        //   assertion handler will cause 'terminate' to be called for
        //   violating the exception specification.  Therefore, this test case
        //   will install a custom assert handler directly before calling the
        //   destructor of a (illegally) bitwise-moved object, that calls
        //   'exit(0)' to indicate success, honoring the contract than an
        //   assertion handler should not return to the caller.  As this will
        //   end the process, it must be the last test in this test case, on a
        //   successful run.  Following the destructor, we must ASSERT to
        //   signal an error that the destructor was *not* supposed to return
        //   control back to the test case.
        //
        // Concerns:
        //: 1 Bitwise moving this object will cause the destructor to raise an
        //:   assertion.
        //
        // Plan:
        //: 1 Create an memory buffer and create a new object with
        //:   placement-new.
        //:
        //: 2 Install an assertion handler that will call 'exit' if triggered.
        //:
        //: 3 Bitwise-move the created object to another location in the
        //:   buffer.  Explicitly call the object's destructor, which should
        //:   exit via the assertion handler..  (C-1)
        //:
        //: 4 ASSERT after destruction to highlight an error if the destructor
        //:   returns.
        //
        // Testing:
        //   CONCERN: Bitwise-moved objects assert on destruction
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSERTION ON BITWISE MOVE"
                            "\n=================================\n");
        {


            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj *arr = reinterpret_cast<Obj*>(
                                            scratch.allocate(sizeof(Obj) * 2));
            new (arr) Obj();
            arr[0].~Obj();

            // The next test has an unusual code-path, and will directly call
            // 'exit' from an assertion handler on success.  It must be the
            // last part of this test case, as it will exit the process on
            // success without returning control to this test case.

            if (verbose) printf("\n");
            {
                bsls::AssertFailureHandlerGuard hG(&exitHandler);

                new (arr) Obj();
                memmove(arr+1, arr, sizeof(Obj));
                arr[1].~Obj();     // This should 'exit' if asserts are enabled

                ASSERTV("The preceding destructor should 'exit' the process",
                        false);
            }

            scratch.deallocate(reinterpret_cast<void*>(arr));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   Ensure that 'AllocTestType' has the necessary trait values to
        //   guarantee its expected behavior.
        //
        // Concerns:
        //: 1 The object has the 'bslma::UsesBslmaAllocator' trait.
        //:
        //: 2 The object is not bitwise-moveable.
        //
        // Plan:
        //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
        //
        // Testing:
        //   CONCERN: The object has the necessary type traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        BSLMF_ASSERT( bslma::UsesBslmaAllocator<Obj>::value);
        BSLMF_ASSERT(!bslmf::IsBitwiseMoveable<Obj>::value);
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
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   AllocTestType& operator=(const AllocTestType& rhs);
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1 = VALUES[ti].d_line;
            const int DATA1 = VALUES[ti].d_data;

            if (veryVerbose) { T_ P_(LINE1) P(DATA1) }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z(DATA1, &scratch);
            const Obj ZZ(DATA1, &scratch);

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2 = VALUES[tj].d_line;
                const int DATA2 = VALUES[tj].d_data;

                if (veryVerbose) { T_ T_ P_(LINE2) P(DATA2) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(DATA2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X.data()) }

                    ASSERTV(LINE1, LINE2, Z.data(), X.data(),
                            (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2, Z.data(), X.data(), Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

                    ASSERTV(LINE1, LINE2, ZZ.data(), Z.data(), ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    ASSERTV(LINE1, LINE2, oam.isInUseSame());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                      Obj mX(DATA1, &oa);
                const Obj ZZ(DATA1, &scratch);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ.data(), Z.data(), ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1, ZZ.data(), Z.data(), ZZ == Z);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINE1, oam.isInUseSame());

                ASSERTV(LINE1, sam.isInUseSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
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
        // COPY CONSTRUCTORS
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 The copy constructor is exception-neutral w.r.t. memory
        //:   allocation.
        //:
        //: 8 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 9 The value of the original object is unchanged.
        //:
        //:10 The allocator address held by the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..10)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor in the presence of injected
        //:     exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:     macros) on 'Z' from P-2.1, but configured differently: (a)
        //:     without passing an allocator, (b) passing a null allocator
        //:     address explicitly, and (c) passing the address of a test
        //:     allocator distinct from the default.  (C-7)
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..10)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-8)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 9)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-9
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator address that it holds is
        //:       unchanged.  (C-6, 10)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8)
        //:
        //:       1 An object allocates memory from the object allocator only.
        //:         (C-2..4)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //
        // Testing:
        //   AllocTestType(const AllocTestType& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            const int DATA = VALUES[ti].d_data;

            if (veryVerbose) { T_ P_(LINE) P(DATA) }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z(DATA, &scratch);
            const Obj ZZ(DATA, &scratch);


            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;

                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(CONFIG, tam.isInUseUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

                if (veryVerbose) { T_ P_(LINE) P(DATA) }

                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;


                ASSERTV(Z.data(), X.data(), Z == X);
                ASSERTV(Z.data(), ZZ.data(), Z == ZZ);


                // Verify any attribute allocators are installed properly.
                ASSERTV(LINE, CONFIG,
                             &oa == X.allocator());

                // Also invoke the object's 'allocator' accessor, as well as
                // that of 'Z'.

                ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                             &oa == X.allocator());

                ASSERTV(LINE, CONFIG, &scratch, Z.allocator(),
                             &scratch == Z.allocator());

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                             0 == noa.numBlocksTotal());

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                             oa.numBlocksInUse(),
                             oa.numBlocksTotal() == oa.numBlocksInUse());

                // Verify expected object-memory allocations.

                ASSERTV(LINE, CONFIG, 1, oa.numBlocksInUse(),
                             oa.numBlocksInUse() == 1);


                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                             0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
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
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10, 12..13)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 4 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-2..3)
        //:
        //:   2 Verify that no memory is ever allocated after object
        //:     construction.  (C-11)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
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
        //:
        //:     4 Verify that no memory is ever allocated after object
        //:       construction.  (C-11)
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

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void) operatorEq;  // quash potential compiler warnings
            (void) operatorNe;
        }


        // Create a test allocator and install it as the default.

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1 = VALUES[ti].d_line;
            const int DATA1 = VALUES[ti].d_data;

            if (veryVerbose) { T_ P_(LINE1) P(DATA1) }

            Obj mX(DATA1); const Obj& X = mX;

            bslma::TestAllocatorMonitor tam(&da);

            // Ensure an object compares correctly with itself (alias test).
            ASSERTV(X.data(),   X == X);
            ASSERTV(X.data(), !(X != X));

            ASSERT(tam.isTotalSame());

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2 = VALUES[tj].d_line;
                const int DATA2 = VALUES[tj].d_data;

                bool EXP = ti == tj;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(DATA2) P(EXP) }

                Obj mY(DATA2); const Obj& Y = mY;

                bslma::TestAllocatorMonitor tam(&da);

                // Verify value, commutativity
                ASSERTV(X.data(), Y.data(),  EXP == (X == Y));
                ASSERTV(X.data(), Y.data(),  EXP == (Y == X));

                ASSERTV(X.data(), Y.data(), !EXP == (X != Y));
                ASSERTV(X.data(), Y.data(), !EXP == (Y != X));

                ASSERT(tam.isTotalSame());
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
        //:
        //: 3 No accessor allocates any memory.
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
        //:
        //: 3 Verify that no memory is ever allocated after default
        //:   construction.  (C-3)
        //
        // Testing:
        //   int data() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS"
                   "\n===============\n");

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX; const Obj& X = mX;

        bslma::TestAllocatorMonitor tam(&da);
        ASSERTV(X.data(), 0 == X.data());

        mX.setData(1);
        ASSERTV(X.data(), 1 == X.data());

        ASSERT(tam.isTotalSame());
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
        //:
        //: 2 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table of P-1:
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value of 'R1', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros)
        //:       with its object allocator configured appropriately (see
        //:       P-2.1); use a distinct test allocator for the object's
        //:       footprint.  (C-9)
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 5)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-6)
        //:
        //:     5 Use the appropriate test allocators to verify that:
        //:       (C-2..4, 7..8)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2..4)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //
        // Testing:
        //   AllocTestType(int data);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            const int DATA = VALUES[ti].d_data;

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;

                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return testStatus;                                // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(DATA);
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DATA, 0);
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DATA, &sa);
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return testStatus;                            // RETURN
                    }
                    ASSERTV(CONFIG, tam.isInUseUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

                if (veryVerbose) { T_ P_(LINE) P(DATA) }

                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Verify the object's attribute values.

                ASSERTV(CONFIG, DATA, X.data(), DATA == X.data());

                // Verify any attribute allocators are installed properly.

                ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                // Verify no allocation from the non-object allocators.

                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
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
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The default constructor is exception-neutral w.r.t. memory
        //:   allocation.
        //:
        //:10 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three attribute values for the 'data' attribute 'D', 'A',
        //:   and 'B'.  'D' should be the default value.  'A' and 'B' should be
        //:   the the boundary values.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object 'X'
        //:     in the presence of exception (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros), with its object
        //:     allocator configured appropriately (see P-2); use a distinct
        //:     test allocator for the object's footprint.  (C-9)
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that the
        //:     appropriate amount of memory is allocated by the default
        //:     constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 Set 'data' attribute to 'A', 'B', and 'D'.  Verify that no
        //:     memory allocation occurs.  Use the individual (as yet
        //:     unproven) salient attribute accessors to verify the attribute
        //:     has been changed.  (C-5..6)
        //:
        //:   7 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   8 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //
        // Testing:
        //   AllocTestType(bslma::Allocator *bA = 0);
        //   ~AllocTestType();
        //   void setData(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                            "\n===================================\n");

        const int D = 0;
        const int A = INT_MIN;
        const int B = INT_MAX;

        if (verbose)
            printf("\nTesting with various allocator configurations.\n");

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return testStatus;                                    // RETURN
            }

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                bslma::TestAllocatorMonitor tam(&oa);
                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj();
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(0);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(&sa);
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return testStatus;                                // RETURN
                }
                ASSERTV(CONFIG, tam.isInUseUp());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the non-object allocators.

            ASSERTV(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Verify the object's attribute values.

            ASSERTV(CONFIG, D, X.data(), D == X.data());


            // 'data'
            {
                bslma::TestAllocatorMonitor tam(&oa);

                mX.setData(A);
                ASSERTV(CONFIG, A == X.data());

                mX.setData(B);
                ASSERTV(CONFIG, B == X.data());

                mX.setData(D);
                ASSERTV(CONFIG, D == X.data());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                         oa.numBlocksTotal() == oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj X;
        ASSERT(X.data() == 0);

        X.setData(1);
        ASSERT(X.data() == 1);

        Obj Y(2);
        ASSERT(Y.data() == 2);

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

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
