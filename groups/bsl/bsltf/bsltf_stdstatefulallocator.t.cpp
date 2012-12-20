// bsltf_stdstatefulallocator.t.cpp                                   -*-C++-*-
#include <bsltf_stdstatefulallocator.h>

#include <bsltf_simpletesttype.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <limits>
#include <stdlib.h>
#include <stdio.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a value-semantic type,
// 'StdStatefulAllocator', a utility, 'StdStatefulAllocatorConfiguration', and a
// mechanism, 'StdStatefulAllocatorConfigurationGuard'.  'StdStatefulAllocator' holds
// no internal state and delegate its operations to a static 'bslma_Allocator'
// object referred by 'StdStatefulAllocatorConfiguration', which provides static
// methods to access and manipulate the static pointer to that delegate
// allocator.  'StdStatefulAllocatorConfigurationGuard' provides a scoped guard
// that temporarily replace the delegate allocator with a user specified
// allocator.
//
// The fact that 'StdStatefulAllocator' doesn't hold an internal state means that
// we will differ from our usual method to test value-semantic types.
// Specifically, instead of testing the primary manipulators and basic
// accessors of 'StdStatefulAllocator', we will instead test the class methods
// provided by 'StdStatefulAllocatorConfiguration'.
//
// In addition, many test cases can be relaxed or made trivial, such as the
// test cases for the copy constructor, assignment operator, and equality
// comparison operators.
//-----------------------------------------------------------------------------
// class StdStatefulAllocator
//
// PUBLIC TYPES
// [11] size_type;
// [11] difference_type;
// [11] pointer;
// [11] const_pointer;
// [11] reference;
// [11] const_reference;
// [11] value_type;
// [11] rebind<U>::other;
// [  ] propagate_on_container_copy_assignment
// [  ] propagate_on_container_move_assignment
// [  ] propagate_on_container_swap
//
// CREATORS
// [ 2] StdStatefulAllocator(bslma::allocator *);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator& original);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator<OTHER>&);
// [ 2] ~StdStatefulAllocator();
//
// MANIPULATORS
// [ 9] StdStatefulAllocator& operator=(const StdStatefulAllocator& rhs);
// [12] pointer allocate(size_type numElements, const void *hint = 0);
// [12] void deallocate(pointer address, size_type numElements = 1);
// [13] void construct(pointer address, const TYPE& value);
// [13] void destroy(pointer address);
//
// ACCESSORS
// [14] pointer address(reference object) const;
// [14] const_pointer address(const_reference object) const;
// [15] size_type max_size() const;
// [  ] bslma::TestAllocator *testAllocator() const;
// [  ] StdStatefulAllocator select_on_container_copy_construction() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
// [ 6] bool operator!=(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [11] TYPEDEFS
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}


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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef StdStatefulAllocator<int>              Obj;
typedef StdStatefulAllocator<float>            ObjF;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing The Support for STL-Compliant Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will verify that a type supports the use of a
// STL-compliant allocator.
//
// First we define a simple container type intended to be used with a C++03
// standard compliant allocator:
//..
template <class TYPE, class ALLOCATOR>
class MyContainer {
    // This container type is parameterized on a standard allocator type and
    // contains a single object, always initialized, which can be replaced and
    // accessed.

    // DATA MEMBERS
    ALLOCATOR  d_allocator;  // allocator used to supply memory (held, not
                             // owned)

    TYPE      *d_object_p;   // pointer to the contained object

  public:
    // CONSTRUCTORS
    MyContainer(const TYPE& object, const ALLOCATOR& allocator);
        // Create an container containing the specified 'object', using the
        // parameterized 'ALLOCATOR' to allocate memory.

    ~MyContainer();
        // Destroy this container.

    // MANIPULATORS
    TYPE& object();
        // Return a reference providing modifiable access to the object
        // contained in this container.

    // ACCESSORS
    const TYPE& object() const;
        // Return a reference providing non-modifiable access to the object
        // contained in this container.
};
//..
// Then, we define the member functions of 'MyContainer':
//..
// CREATORS
template <class TYPE, class ALLOCATOR>
MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE& object,
                                          const ALLOCATOR& allocator)
: d_allocator(allocator)
{
    d_object_p = d_allocator.allocate(1);
    d_allocator.construct(d_object_p, object);
}

template <class TYPE, class ALLOCATOR>
MyContainer<TYPE, ALLOCATOR>::~MyContainer()
{
    d_allocator.destroy(d_object_p);
    d_allocator.deallocate(d_object_p);
}

// MANIPULATORS
template <class TYPE, class ALLOCATOR>
TYPE& MyContainer<TYPE, ALLOCATOR>::object()
{
    return *d_object_p;
}

// ACCESSORS
template <class TYPE, class ALLOCATOR>
const TYPE& MyContainer<TYPE, ALLOCATOR>::object() const
{
    return *d_object_p;
}

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

namespace {

bool s_destroyedFlag = false;

class TestType {
    // DATA
    int d_data;

  public:
    // CREATORS
    TestType(int data)
    : d_data(data)
    {
    }

    ~TestType()
    {
        s_destroyedFlag = true;
    }

    // ACCESSORS
    int data()
    {
        return d_data;
    }
};

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3;
    int veryVeryVerbose     = argc > 4;
    int veryVeryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

//..
// Now, we use 'StdStatefulAllocator' to implement a simple test for 'MyContainer'
// to verify it correctly uses a parameterized allocator using only the C++03
// standard methods:
//..
          bslma::TestAllocator oa("object", veryVeryVeryVerbose);
          {
              typedef MyContainer<int, StdStatefulAllocator<int> > Obj;

              Obj mX(2, StdStatefulAllocator<int>(&oa)); const Obj& X = mX;
              ASSERT(sizeof(int) == oa.numBytesInUse());

              ASSERT(X.object() == 2);

              mX.object() = -10;
              ASSERT(X.object() == -10);
          }

          ASSERT(0 == oa.numBytesInUse());
//..
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // 'max_size'
        //
        // Concerns:
        //: 1 The result of the 'max_size' method fits and represents the
        //:   maximum possible number of bytes in a
        //:   'bslma_Allocator::size_type'.
        //
        // Plan:
        //: 1 Use 'std::numeric_limits' to verify that the value return by
        //:   'max_size' is the largest possible value for 'size_type'.
        //
        // Testing:
        //   size_type max_size() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\'max_size'"
                            "\n=========\n");

        // This part has been copied from bslstl_allocator's test driver and
        // had been originally written by Pablo.

        typedef bslma::Allocator::size_type bsize;

        enum {
            BSLMA_SIZE_IS_SIGNED = ~bslma::Allocator::size_type(0) < 0,
            MAX_NUM_BYTES = ~std::size_t(0) /
            (BSLMA_SIZE_IS_SIGNED ? 2 : 1),
            MAX_ELEMENTS1 = MAX_NUM_BYTES / sizeof(char),
            MAX_ELEMENTS2 = (std::size_t)MAX_NUM_BYTES / sizeof(char)
        };

        if (verbose) {
            printf("Illustrating the reason for the cast in the"
                   " enumeration (on AIX 64-bit mode):\n");
            printf("\tBSLMA_SIZE_IS_SIGNED = %d\n", BSLMA_SIZE_IS_SIGNED);
            printf("\tMAX_NUM_BYTES = %ld\n", (bsize)MAX_NUM_BYTES);
            printf("\tMAX_ELEMENTS1 = %ld\n", (bsize)MAX_ELEMENTS1);
            printf("\tMAX_ELEMENTS2 = %ld\n", (bsize)MAX_ELEMENTS2);

            printf("Printing the same values as unsigned:\n");
            printf("\tBSLMA_SIZE_IS_SIGNED = %d\n", BSLMA_SIZE_IS_SIGNED);
            printf("\tMAX_NUM_BYTES = %lu\n", (bsize)MAX_NUM_BYTES);
            printf("\tMAX_ELEMENTS1 = %lu\n", (bsize)MAX_ELEMENTS1);
            printf("\tMAX_ELEMENTS2 = %lu\n", (bsize)MAX_ELEMENTS2);
        }

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        StdStatefulAllocator<char> X(&oa);
        bsize cas = X.max_size();

        ASSERTV(cas, cas > 0);
        ASSERTV(cas, std::numeric_limits<bsize>::max(),
                cas == std::numeric_limits<bsize>::max());

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'address'
        //
        // Concerns:
        //: 1 The 'address' method with a return type 'const_pointer' returns a
        //:   const pointer of the passed-in modifiable reference of the
        //:   parameterized type.
        //:
        //: 2 The 'address' method with a return type 'pointer' returns a const
        //:   pointer of the passed-in non-modifiable reference of the
        //:   parameterized type.
        //
        // Plan:
        //
        // Testing:
        //   pointer address(reference object) const;
        //   const_pointer address(const_reference object) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nADDRESS"
                            "\n=======\n");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj sta(&oa);

        int a(1);
        int& mA = a;
        const int& A = a;

        int* mAPtr = sta.address(mA);
        const int* APtr = sta.address(A);

        ASSERT(mAPtr == &a);
        ASSERT(APtr == &a);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'construct' AND 'destroy'
        //
        // Concerns:
        //: 1 The 'construct' method copy-construct the object of the
        //:   parameterized type at a specified memory 'address'.
        //:
        //: 2 The 'destroy' method calls the destructor for the object of the
        //:   parameterized type at a specified memory 'address'.
        //
        // Plan:
        //: 1 Create a 'StdStatefulAllocator' object parameterized on a test type.
        //:   Use the object to allocate memory required for one object of the
        //:   test type.
        //:
        //: 2 Construct a test type object with a unique value.  Use the
        //:   'construct' method to copy-construct the just created test object
        //:   to the address of the memory block allocated in P-1.  Verify that
        //:   the copy-constructed test object has the same value as the
        //:   original object.  (C-1)
        //:
        //: 3 Use the 'destroy' method to destroy the copy-constructed object
        //:   in P-2.  Verify that the destructor of the object has been called
        //:   by checking a global flag set in the destructor.  (C-2)
        //
        // Testing:
        //   void construct(pointer address, const TYPE& value);
        //   void destroy(pointer address);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'construct' AND 'destroy'"
                            "\n=========================\n");

        bslma::TestAllocator oa;

        StdStatefulAllocator<TestType> sta(&oa);

        TestType *testTypePtr = sta.allocate(1);

        TestType X(123);

        sta.construct(testTypePtr, X);
        ASSERTV(testTypePtr->data(), 123 == testTypePtr->data());

        s_destroyedFlag = false;
        sta.destroy(testTypePtr);
        ASSERTV(s_destroyedFlag, true == s_destroyedFlag);

        sta.deallocate(testTypePtr);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'allocate' AND 'deallocate'
        //
        // Concerns:
        //: 1 The 'allocate' method forwards allocation requests to the
        //:   approporiate delegate allocator.
        //:
        //: 2 The 'deallocate' method forwards the deallocation requests to the
        //:   approporiate delegate allocator.
        //
        // Plan:
        //: 1 Create a 'bslma_Allocator' object and install it as the delegate
        //:   allocator for 'StdStatefulAllocator'.
        //:
        //: 2 Create a new 'StdStatefulAllocator' object and invoke the 'allocate'
        //:   method.  Verify that the correct amount of memory has been
        //:   allocated from the delegate allocator.  (C-1)
        //:
        //: 3 Invoke the 'deallocate' method and verify that the correct amount
        //:   of memory has been deallocated from the delegate allocator.
        //:   (C-2)
        //
        // Testing:
        //   pointer allocate(size_type numElements, const void *hint = 0);
        //   void deallocate(pointer address, size_type numElements = 1);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'allocate' AND 'deallocate'"
                            "\n===========================\n");

        bslma::TestAllocator oa;

        Obj sta(&oa);
        int *ptr = sta.allocate(1);

        ASSERTV(oa.numBytesInUse(), sizeof(int) == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), sizeof(int) == oa.numBytesTotal());

        sta.deallocate(ptr);

        ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), sizeof(int) == oa.numBytesTotal());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // NESTED TYPES
        //
        // Concerns:
        //: 1 The 'typedef' aliases defined in this component are as specified
        //:   by the C++03 standard for template instances parameterized on the
        //:   'void' and other types .
        //:
        //: 2 'size_type' is unsigned while 'difference_type' is signed.
        //:
        //: 3 'rebind<OTHER_TYPE>::other' defines a template instance for
        //:   'StdStatefulAllocator' parameterized on the 'OTHER_TYPE' type.
        //
        // Plan:
        //: 1 Define three aliases for 'StdStatefulAllocator' parameterized on
        //:   'int', 'float', and 'void' types.
        //:
        //: 2 For each alias defines in P-1:
        //:
        //:   1 Use the 'sizeof' operator to verify that 'size_type' and
        //:     'difference_type' are the right size and verify they are
        //:     unsigned values.  (C-1..2)
        //:
        //:   2 For all other type aliases, use 'bsl::is_same' to verify that
        //:     they are the expected types, except for 'reference' and
        //:     'const_reference' for the instance parameterized on the 'void'
        //:     type.  (C-1)
        //:
        //:   3 Verify using 'bsl::is_same' that 'rebind<U>::other', where 'U'
        //:     is the other two aliases defined by P-1, defines the correct
        //:     type.  (C-3)
        //
        // Testing:
        //   size_type
        //   difference_type
        //   pointer;
        //   const_pointer;
        //   reference;
        //   const_reference;
        //   value_type;
        //   rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nNESTED TYPES"
                            "\n============\n");

        typedef StdStatefulAllocator<int>   AI;
        typedef StdStatefulAllocator<float> AF;

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT(sizeof(AI::size_type) == sizeof(int*));

            ASSERT(0 < ~(AI::size_type)0);
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT(sizeof(AI::difference_type) == sizeof(int*));

            ASSERT(0 > ~(AI::difference_type)0);
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bsl::is_same<AI::reference, int&>::value));
            ASSERT((bsl::is_same<AF::reference, float&>::value));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference, const int&>::value));
            ASSERT((bsl::is_same<AF::const_reference, const float&>::value));
        }

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
        }
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
        //   Since 'StdStatefulAllocator' doesn't hold any state and provides the
        //   compiler supplied assignment operator, there are no concerns
        //   beyond the signatures are correctly defined.
        //
        // Concerns:
        //: 1 The signature and return type are standard.
        //:
        //: 2 Assignment operator can assign any modifiable target object to
        //:   any source object.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-1)
        //:
        //: 2 Create two sets of 'StdStatefulAllocator' objects (parameterized on
        //:   void and int) and assign a non-modifiable refernce of one to the
        //:   other.  (C-2)
        //
        // Testing:
        //   StdStatefulAllocator& operator=(const StdStatefulAllocator& rhs);
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

        bslma::TestAllocator allocW("W");
        bslma::TestAllocator allocX("X");
        bslma::TestAllocator allocY("Y");
        bslma::TestAllocator allocZ("Z");

        Obj mW(&allocW), mX(&allocX); const Obj& X = mX;
        mW = X;

        ObjF mY(&allocY), mZ(&allocZ); const ObjF& Z = mZ;
        mY = Z;

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
        //   Since 'StdStatefulAllocator' doesn't hold any state and provides the
        //   compiler supplied copy constructor, there are no concerns beyond
        //   the signatures are correctly defined.
        //
        // Concerns:
        //: 1 A 'StdStatefulAllocator' object can be copy constructed from a const
        //:   reference of another object of the same type.
        //:
        //: 2 A 'StdStatefulAllocator' object can be copy constructed from a const
        //:   reference of another object of the 'StdStatefulAllocator' template
        //:   instance parameterized on a different type.
        //
        // Plan:
        //: 1 Create a 'StdStatefulAllocator' object.  Use a const reference of the
        //:   object to copy construct another object of the same type.  (C-1)
        //:
        //: 2 Use the object created in P-1 to copy construct an object of the
        //:   'StdStatefulAllocator' template instant parameterized on a different
        //:   type.  (C-2)
        //
        // Testing:
        //   StdStatefulAllocator(const StdStatefulAllocator& original);
        //   StdStatefulAllocator(const StdStatefulAllocator<OTHER>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        bslma::TestAllocator allocX("X");

        Obj mX(&allocX); const Obj& X = mX;
        Obj mY(X);

        ObjF Z(X);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 'operator==' always return true, even for objects of different
        //:   template instances.
        //:
        //: 2 'operator!=' always return false, enve for objects of different
        //:   template instances.
        //
        //: 3 The equality operator's signature and return type are standard.
        //:
        //: 4 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-3..4)
        //
        //: 2 Create a few 'StdStatefulAllocator' objects of different template
        //:   instances.  Verify invoking 'operator==' returns true and
        //:   invoking 'operator!=' returns false.  (C-1..2)
        //
        // Testing:
        //   bool operator==(const StdStatefulAllocator<TYPE>& lhs, rhs);
        //   bool operator!=(const StdStatefulAllocator<TYPE>& lhs, rhs);
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

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        bslma::TestAllocator allocX("X");
        bslma::TestAllocator allocY("Y");
        bslma::TestAllocator allocZ("Z");

        const Obj X(&allocX);
        const Obj Y(&allocY);
        const Obj Z(&allocZ);

        bslma::TestAllocator allocA("A");

        const ObjF A(&allocA);


        ASSERT(X == X);
        ASSERT(X != Y);
        ASSERT(!(X == Y));
        ASSERT(!(X != X));
//        ASSERT(X == Z);
//        ASSERT(!(X != Z));

        ASSERT(A == A);
        ASSERT(!(A != A));
//        ASSERT(X == A);
//        ASSERT(A == B);
//        ASSERT(!(X != B));

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
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing the test machinery
        //
        // Concerns:
        //: 1 ....
        //
        // Plan:
        //: 1 ....  (C-1)
        //:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting the test machinery"
                            "\n==========================");
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Value constuctor and basic manipulators (if any)
        //
        // Concerns:
        //: 1 The 'StdStatefulAllocatorConfiguration::setDelegateAllocatorRaw'
        //:   class method set the allocator to which 'StdStatefulAllocator'
        //:   delegates.
        //:
        //: 2 The 'StdStatefulAllocatorConfiguration::delegateAllocator' class
        //:   method returns the current delegate allocator.
        //
        // Plan:
        //: 1 Verify that, by default, 'delegateAllocator' returns a pointer
        //:   to the 'bslma_NewDeleteAllocator' singleton.  (C-2)
        //:
        //: 2 Create a 'bslma_TestAllocator' object and invoke
        //:   'setDelegateAllocatorRaw' passing in a pointer to the just
        //:   created allocator.  Verify that 'delegateAllocator' returns the
        //:   pointer passed in the previous call.  (C-1)
        //:
        //: 3 Create a 'StdStatefulAllocator' object and use the (as yet unproven)
        //:   'allocate' method allocates from the just test allocator created
        //:   by P-3.  (C-1)
        //
        // Testing:
        //   StdStatefulAllocator(bslma::TestAllocator *);
        //   ~StdStatefulAllocator();
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nValue constuctor and basic manipulators (if any)"
                   "\n================================================\n");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);
        int* ptr = mX.allocate(2);
        ASSERT(sizeof(int)*2 == oa.numBytesInUse());

        mX.deallocate(ptr);
        ASSERT(0 == oa.numBytesInUse());

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
                            "\n==============");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        {
            StdStatefulAllocator<int> sa(&oa);
            int *ptr = sa.allocate(2);
            ASSERT(oa.numBytesInUse() == 2*sizeof(int));
            sa.deallocate(ptr);
            ASSERT(oa.numBytesInUse() == 0);
        }

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
