// bdlb_nullableallocatedvalue_pointerbitspair.t.cpp                  -*-C++-*-

#include "bdlb_nullableallocatedvalue_pointerbitspair.h"

#include <bslalg_swaputil.h>

#include <bslma_testallocator.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_bsltestutil.h>

#include <bslstl_referencewrapper.h>

#include <bsltf_templatetestfacility.h>

#include <bsl_iostream.h>

#include <climits>  // 'INT_MIN'
#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// TOTO REMOVE UNUSED test cases

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// This component contains a single class template,
// 'bdlb::NullableAllocatedValue_PointerBitsPair', a regular value-semantic
// private class holding a pointer and some number of flags.  The salient
// attributes of a 'NullableAllocatedValue_PointerBitsPair' are the pointer and
// the value of the flags.  There are no non-salient attributes.  The primitive
// manipulators are 'setPointer', which sets the pointer, 'setFlag' and
// 'clearFlag', which manipulate the flags.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] NullableAllocatedValue_PointerBitsPair();
// [ 3] NullableAllocatedValue_PointerBitsPair(TYPE *, unsigned);
// [ 6] NullableAllocatedValue_PointerBitsPair(const NAV_PBP &);
// [ 6] NullableAllocatedValue_PointerBitsPair(MovableRef<NAV_PBP>);
//
// MANIPULATORS
// [ 8] operator=(const NAV_PBP&);
// [ 8] operator=(bslmf::MovableRef<NAV_PBP>);
// [ 2] void clearFlag(unsigned flagIdx);
// [ 2] void setFlag(unsigned flagIdx);
// [ 2] void setPointer(TYPE *);
// [ 7] void swap(NAV_PBP& other);
//
// ACCESSORS
// [ 4] bool readFlag() const;
// [ 4] TYPE *getPointer() const;
// [ 5] bool equal(const NAV_PBP& other) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const NAV_PBP& lhs, const NAV_PBP& rhs);
// [ 5] bool operator!=(const NAV_PBP& lhs, const NAV_PBP& rhs);
//
// FREE FUNCTIONS
// [ 7] void swap(NAV_PBP& a, NAV_PBP &b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

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
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

template <class TYPE, unsigned NUM_BITS>
bool as_expected(
  const bdlb::NullableAllocatedValue_PointerBitsPair<TYPE, NUM_BITS>&  value,
  TYPE                                                                *pointer,
  unsigned                                                             flags)
    // Compare the specified 'value' to the specified 'pointer' and 'flags'.
    // Return 'true' if they match, and 'false' otherwise.
{
    if (value.getPointer() != pointer) {
        return false;                                                 // RETURN
        }

    unsigned mask = 1;
    for (unsigned i = 0; i < NUM_BITS; ++i, mask <<= 1) {
        if (value.readFlag(i) != ((flags & mask) != 0))
            return false;                                             // RETURN
    }
    return true;
}
}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

// No usage example for private component

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    typedef bdlb::NullableAllocatedValue_PointerBitsPair<double, 2> Obj;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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
        //: 2 The value of the source object is not modified.
        //:
        //: 3 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 Since this is a value-semantic object, it does not support
        //:  move-assignment. However, we need to ensure that assignment from a
        //:  rvalue-reference works.
        //:
        //: 7 No memory is allocated.
        //:
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a pair of objects, and give them different states. Assign
        //:   one to the other, and check to see that the state of the
        //:   destination object is modified, and that the state of the source
        //:   object is unchanged. (C-1, C-2)
        //:
        //: 3 Assign an object to itself, and verify that the state of the
        //:   object is unchanged. (C-3)
        //:
        //: 4 Take the address of the return value of the assignment, and
        //:   verify that it is the same as the address of the destination.
        //:   (C-5)
        //:
        //: 5 Assign an object from a MovableRef to another object, and verify
        //:   that the state of the destination object is updated, and that the
        //:   state of the source object is unchanged. (C-6)
        //:
        //: 6 Use the installed test allocator to verify that no memory is ever
        //:   allocated from the default allocator.  (C-7)
        //
        // Testing:
        //   operator=(const NAV_PBP&);
        //   operator=(bslmf::MovableRef<NAV_PBP>);
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

        double d1 = 2.3;
        double d2 = 4.6;
        double d3 = 6.9;
        Obj    o1(&d1, 1);  const Obj &O1(o1);
        Obj    o2(&d2, 2);  const Obj &O2(o2);
        Obj    o3(&d3, 3);  const Obj &O3(o3);

        // basic sanity check
        ASSERT(as_expected(O1, &d1, 1));
        ASSERT(as_expected(O2, &d2, 2));
        ASSERT(as_expected(O3, &d3, 3));

        // assignment
        Obj *p3 = &(o3 = O1);
        ASSERT(as_expected(O1, &d1, 1));
        ASSERT(as_expected(O3, &d1, 1));
        ASSERT(p3 == &o3);

        // self-assignment
        o2 = O2;
        ASSERT(as_expected(O2, &d2, 2));

        // assignment from rvalue
        o2 = bslmf::MovableRefUtil::move(o3);
        ASSERT(as_expected(O3, &d1, 1));
        ASSERT(as_expected(O2, &d1, 1));

        ASSERT(0 == globalAllocator.numAllocations());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 No memory is allocated.
        //:
        //: 3 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 4 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 5 The signature and return type are standard.
        //:
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 1 Create a pair of objects, and give them different states. Swap
        //:   one with the other, and check to see that the state of the source
        //:   and the state of the destination are exchanged.
        //:   (C-1)
        //:
        //: 2 Swap an object with itself, and verify that the state of the
        //:   object is unchanged. (C-3)
        //:
        //: 3 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:   values of two object, using the free 'swap' function defined in
        //:   this component, then verify that the values have been exchanged.
        //:   (C-4)
        //:
        //: 4 Use the installed test allocator to verify that no memory is ever
        //:   allocated from the default allocator.  (C-2)
        //
        // Testing:
        //   void swap(NAV_PBP& other);
        //   void swap(NAV_PBP& a, NAV_PBP &b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = bdlb::swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        double d1 = 2.3;
        double d2 = 4.6;
        double d3 = 6.9;
        Obj    o1(&d1, 1);  const Obj &O1(o1);
        Obj    o2(&d2, 2);  const Obj &O2(o2);
        Obj    o3(&d3, 3);  const Obj &O3(o3);

        // basic sanity check
        ASSERT(as_expected(O1, &d1, 1));
        ASSERT(as_expected(O2, &d2, 2));
        ASSERT(as_expected(O3, &d3, 3));

        // swap
        bdlb::swap(o1, o3);
        ASSERT(as_expected(O1, &d3, 3));
        ASSERT(as_expected(O3, &d1, 1));

        o1.swap(o3);
        ASSERT(as_expected(O1, &d1, 1));
        ASSERT(as_expected(O3, &d3, 3));

        // self-swap
        bdlb::swap(o2, o2);
        ASSERT(as_expected(O2, &d2, 2));

        o2.swap(o2);
        ASSERT(as_expected(O2, &d2, 2));

        // ADL check
        bslalg::SwapUtil::swap(&o1, &o3);
        ASSERT(as_expected(O1, &d3, 3));
        ASSERT(as_expected(O3, &d1, 1));

        // self-swap
        bslalg::SwapUtil::swap(&o2, &o2);
        ASSERT(as_expected(O2, &d2, 2));

        ASSERT(0 == globalAllocator.numAllocations());
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
        //: 2 The original object is passed as a 'const' reference.
        //:
        //: 3 The value of the original object is unchanged.
        //:
        //: 4 Since this is a value-semantic object, it does not support
        //:  move-assignment. However, we need to ensure that assignment from a
        //:  rvalue-reference works.
        //:
        //: 5 No memory is allocated.
        //
        // Plan:
        //: 1 Create an object, and give it a known state. Construct another
        //:   object from a const reference to the first, and verify that the
        //:   new object has the same state as the original and the state of
        //:   the original is unchanged. (C-1, C-2, C-3)
        //:
        //: 2 Use the installed test allocator to verify that no memory is ever
        //:   allocated from the default allocator.  (C-5)
        //
        // Testing:
        //   NullableAllocatedValue_PointerBitsPair(const NAV_PBP &);
        //   NullableAllocatedValue_PointerBitsPair(MovableRef<NAV_PBP>);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        double d1 = 2.3;
        double d2 = 4.6;
        Obj    o1(&d1, 1);  const Obj &O1(o1);
        Obj    o2(&d2, 2);  const Obj &O2(o2);

        // basic sanity check
        ASSERT(as_expected(O1, &d1, 1));
        ASSERT(as_expected(O2, &d2, 2));

        Obj oX(O1);
        ASSERT(as_expected(oX, &d1, 1));
        ASSERT(as_expected(O1, &d1, 1));

        Obj oZ(bslmf::MovableRefUtil::move(O1));
        ASSERT(as_expected(oZ, &d1, 1));
        ASSERT(as_expected(O1, &d1, 1));

        ASSERT(0 == globalAllocator.numAllocations());
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
        //: 3 No non-salient attributes participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:10 No memory is allocated.
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-11)
        //:
        //: 2 Create several objects with both different and the same values,
        //:   and compare both the actual objects, and const references to the
        //:   objects.   Ensure that they compare in the same manner.  (C-1..9)
        //:
        //: 3 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-10)
        //
        // Testing:
        //   bool equal(const NAV_PBP& other) const;
        //   bool operator==(const NAV_PBP& lhs, const NAV_PBP& rhs);
        //   bool operator!=(const NAV_PBP& lhs, const NAV_PBP& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdlb;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        double d1 = 2.3;
        double d3 = 4.6;
        Obj    o1(&d1, 1);  const Obj &O1(o1);
        Obj    o2(&d1, 2);  const Obj &O2(o2);
        Obj    o3(&d3, 1);  const Obj &O3(o3);
        Obj    o4(&d3, 2);  const Obj &O4(o4);

        Obj    o5(&d1, 1);  const Obj &O5(o5);  // same as o1

        // non-const objects
        ASSERT( (o1 == o1)); // same object (C-4)
        ASSERT(!(o1 == o2)); // same pointer, different flags
        ASSERT(!(o1 == o3)); // different pointer, same flags
        ASSERT(!(o1 == o4)); // different pointer, different flags
        ASSERT( (o1 == o5)); // same pointer, same flags

        ASSERT(!(o1 != o1)); // same object (C-5)
        ASSERT( (o1 != o2)); // same pointer, different flags
        ASSERT( (o1 != o3)); // different pointer, same flags
        ASSERT( (o1 != o4)); // different pointer, different flags
        ASSERT(!(o1 != o5)); // same pointer, same flags

        ASSERT( o1.equal(o1)); // same object (C-4)
        ASSERT(!o1.equal(o2)); // same pointer, different flags
        ASSERT(!o1.equal(o3)); // different pointer, same flags
        ASSERT(!o1.equal(o4)); // different pointer, different flags
        ASSERT( o1.equal(o5)); // same pointer, same flags

        // const references
        ASSERT( (O1 == O1)); // same object (C-4)
        ASSERT(!(O1 == O2)); // same pointer, different flags
        ASSERT(!(O1 == O3)); // different pointer, same flags
        ASSERT(!(O1 == O4)); // different pointer, different flags
        ASSERT( (O1 == O5)); // same pointer, same flags

        ASSERT(!(O1 != O1)); // same object (C-5)
        ASSERT( (O1 != O2)); // same pointer, different flags
        ASSERT( (O1 != O3)); // different pointer, same flags
        ASSERT( (O1 != O4)); // different pointer, different flags
        ASSERT(!(O1 != O5)); // same pointer, same flags

        ASSERT( O1.equal(O1)); // same object (C-4)
        ASSERT(!O1.equal(O2)); // same pointer, different flags
        ASSERT(!O1.equal(O3)); // different pointer, same flags
        ASSERT(!O1.equal(O4)); // different pointer, different flags
        ASSERT( O1.equal(O5)); // same pointer, same flags

        // commutativity - non-const references
        ASSERT( (o1 == o1)); // same object (C-4)
        ASSERT(!(o2 == o1)); // same pointer, different flags
        ASSERT(!(o3 == o1)); // different pointer, same flags
        ASSERT(!(o4 == o1)); // different pointer, different flags
        ASSERT( (o5 == o1)); // same pointer, same flags

        ASSERT(!(o1 != o1)); // same object (C-5)
        ASSERT( (o2 != o1)); // same pointer, different flags
        ASSERT( (o3 != o1)); // different pointer, same flags
        ASSERT( (o4 != o1)); // different pointer, different flags
        ASSERT(!(o5 != o1)); // same pointer, same flags

        ASSERT( o1.equal(o1)); // same object (C-4)
        ASSERT(!o2.equal(o1)); // same pointer, different flags
        ASSERT(!o3.equal(o1)); // different pointer, same flags
        ASSERT(!o4.equal(o1)); // different pointer, different flags
        ASSERT( o5.equal(o1)); // same pointer, same flags

        // commutativity - const references
        ASSERT( (O1 == O1)); // same object (C-4)
        ASSERT(!(O2 == O1)); // same pointer, different flags
        ASSERT(!(O3 == O1)); // different pointer, same flags
        ASSERT(!(O4 == O1)); // different pointer, different flags
        ASSERT( (O5 == O1)); // same pointer, same flags

        ASSERT(!(O1 != O1)); // same object (C-5)
        ASSERT( (O2 != O1)); // same pointer, different flags
        ASSERT( (O3 != O1)); // different pointer, same flags
        ASSERT( (O4 != O1)); // different pointer, different flags
        ASSERT(!(O5 != O1)); // same pointer, same flags

        ASSERT( O1.equal(O1)); // same object (C-4)
        ASSERT(!O2.equal(O1)); // same pointer, different flags
        ASSERT(!O3.equal(O1)); // different pointer, same flags
        ASSERT(!O4.equal(O1)); // different pointer, different flags
        ASSERT( O5.equal(O1)); // same pointer, same flags

        ASSERT(0 == globalAllocator.numAllocations());
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
        //:
        //: 3 No accessor allocates any memory.
        //:
        // Plan:
        //: 1 Create two NullableAllocatedValue_PointerBitsPair objects with
        //:   different attributes.
        //:
        //: 2 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-1, returns the expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //: 4 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   bool readFlag() const;
        //   TYPE *getPointer() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        double d1 = 2.3;
        double d2 = 4.6;
        double d3 = 6.9;
        Obj    o1(&d1, 1);  const Obj &O1(o1);
        Obj    o2(&d2, 2);  const Obj &O2(o2);

        ASSERT(&d1 == O1.getPointer());
        ASSERT( O1.readFlag(0));
        ASSERT(!O1.readFlag(1));

        ASSERT(&d2 == O2.getPointer());
        ASSERT(!O2.readFlag(0));
        ASSERT( O2.readFlag(1));

        // set the pointer
        o1.setPointer(&d3);
        ASSERT(&d3 == O1.getPointer());
        ASSERT( O1.readFlag(0));
        ASSERT(!O1.readFlag(1));

        // manipulate the flags
        o1.clearFlag(0);
        ASSERT(&d3 == O1.getPointer());
        ASSERT(!O1.readFlag(0));
        ASSERT(!O1.readFlag(1));

        o1.setFlag(1);
        ASSERT(&d3 == O1.getPointer());
        ASSERT(!O1.readFlag(0));
        ASSERT( O1.readFlag(1));

        ASSERT(0 == globalAllocator.numAllocations());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The value constructor allocates no memory.
        //
        // Plan:
        //: 1 Create several NullableAllocatedValue_PointerBitsPair objects
        //:   with different parameters. Verify that the objects contain the
        //:   expected values.
        //:   (C-1)
        //:
        //: 2 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-2)
        //
        // Testing:
        //   NullableAllocatedValue_PointerBitsPair(TYPE *, unsigned);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        double d1 = 2.3;
        double d2 = 4.6;
        double d3 = 6.9;
        Obj    o1(&d1, 1);
        Obj    o2(&d2, 2);
        Obj    o3(&d3, 3);

        ASSERT(as_expected(o1, &d1, 1));
        ASSERT(as_expected(o2, &d2, 2));
        ASSERT(as_expected(o3, &d3, 3));

        ASSERT(0 == globalAllocator.numAllocations());
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
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 The default constructor allocates no memory.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create a default-constructed object, and verify to make sure that
        //:   all of the attributes have the expected values. (C-1)
        //:
        //: 2 Set each attribute, and afterwards verify that that attribute
        //:   (and none of the others) has been changed as expected. (C-3, 4)
        //:
        //: 3 Verify that no temporary memory is allocated from the default
        //:     allocator.  (C-2)
        //
        // Testing:
        //   NullableAllocatedValue_PointerBitsPair();
        //   void clearFlag(unsigned flagIdx);
        //   void setFlag(unsigned flagIdx);
        //   void setPointer(TYPE *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        Obj p0;
        ASSERT(as_expected(p0, (double *) NULL, 0));

        double d1 = 2.3;
        double d2 = 4.6;

        p0.setPointer(&d1);
        ASSERT(as_expected(p0, &d1, 0));

        p0.setFlag(0);
        ASSERT(as_expected(p0, &d1, 1));

        p0.setFlag(1);
        ASSERT(as_expected(p0, &d1, 3));

        p0.setPointer(&d2);
        ASSERT(as_expected(p0, &d2, 3));

        p0.clearFlag(0);
        ASSERT(as_expected(p0, &d2, 2));

        p0.clearFlag(1);
        ASSERT(as_expected(p0, &d2, 0));

        ASSERT(0 == globalAllocator.numAllocations());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Construct a 'NullableAllocatedValue_PointerBitsPair'.  Verify
        //:   that it is empty.
        //:
        //: 2 Construct a 'NullableAllocatedValue_PointerBitsPair' from a
        //:   pointer.  Verify that  the held pointer is returned.
        //:
        //: 3 Set flags on the objects. Verify that the flags were set and
        //:   cleared correctly.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj p0;
        ASSERT(NULL == p0.getPointer());
        ASSERT(false == p0.readFlag(0));

        double d = 2.3;
        Obj    p1(&d);
        ASSERT(&d == p1.getPointer());
        ASSERT(!p1.readFlag(0));
        ASSERT(!p1.readFlag(1));

        p1.setFlag(0);
        ASSERT( p1.readFlag(0));
        ASSERT(&d == p1.getPointer());
        p1.clearFlag(0);
        ASSERT(!p1.readFlag(0));
        ASSERT(&d == p1.getPointer());

        ASSERT(0 == globalAllocator.numAllocations());
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
