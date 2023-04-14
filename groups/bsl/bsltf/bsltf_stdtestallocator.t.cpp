// bsltf_stdtestallocator.t.cpp                                       -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsltf_simpletesttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <limits>

#include <limits.h>
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
// 'StdTestAllocator', a utility, 'StdTestAllocatorConfiguration', and a
// mechanism, 'StdTestAllocatorConfigurationGuard'.  'StdTestAllocator' holds
// no internal state and delegate its operations to a static 'bslma::Allocator'
// object referred by 'StdTestAllocatorConfiguration', which provides static
// methods to access and manipulate the static pointer to that delegate
// allocator.  'StdTestAllocatorConfigurationGuard' provides a scoped guard
// that temporarily replace the delegate allocator with a user specified
// allocator.
//
// The fact that 'StdTestAllocator' doesn't hold an internal state means that
// we will differ from our usual method to test value-semantic types.
// Specifically, instead of testing the primary manipulators and basic
// accessors of 'StdTestAllocator', we will instead test the class methods
// provided by 'StdTestAllocatorConfiguration'.
//
// In addition, many test cases can be relaxed or made trivial, such as the
// test cases for the copy constructor, assignment operator, and equality
// comparison operators.
//-----------------------------------------------------------------------------
// struct StdTestAllocatorConfiguration
//
// CLASS METHODS
// [ 2] void setDelegateAllocatorRaw(bslma::Allocator *basicAllocator);
// [ 2] bslma::Allocator* delegateAllocator();
//-----------------------------------------------------------------------------
// class StdTestAllocatorConfigurationGuard
//
// CREATORS
// [ 3] StdTestAllocatorConfigurationGuard(bslma::Allocator *temporary);
// [ 3] ~StdTestAllocatorConfigurationGuard();
//-----------------------------------------------------------------------------
// class StdTestAllocator
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
//
// CREATORS
// [ 2] StdTestAllocator();
// [ 7] StdTestAllocator(const StdTestAllocator& original);
// [ 7] StdTestAllocator(const StdTestAllocator<OTHER>&);
// [ 2] ~StdTestAllocator();
//
// MANIPULATORS
// [ 9] StdTestAllocator& operator=(const StdTestAllocator& rhs);
// [12] pointer allocate(size_type numElements, const void *hint = 0);
// [12] void deallocate(pointer address, size_type numElements = 1);
// [13] void construct(pointer address, const TYPE& value);
// [13] void destroy(pointer address);
//
// ACCESSORS
// [14] pointer address(reference object) const;
// [14] const_pointer address(const_reference object) const;
// [15] size_type max_size() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const StdTestAllocator<TYPE>& lhs, rhs);
// [ 6] bool operator!=(const StdTestAllocator<TYPE>& lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [11] TYPEDEFS

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef StdTestAllocator<int>              Obj;
typedef StdTestAllocator<void>             ObjV;
typedef StdTestAllocator<float>            ObjF;
typedef StdTestAllocatorConfiguration      Conf;
typedef StdTestAllocatorConfigurationGuard Grd;

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
    // CREATORS
    explicit MyContainer(const TYPE& object);
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
MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE& object)
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
    explicit TestType(int data)
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

//..
// Now, we use 'StdTestAllocator' to implement a simple test for 'MyContainer'
// to verify it correctly uses a parameterized allocator using only the C++03
// standard methods:
//..
          bslma::TestAllocator oa("object", veryVeryVeryVerbose);
          Grd stag(&oa);
          {
              typedef MyContainer<int, StdTestAllocator<int> > Obj;

              Obj mX(2); const Obj& X = mX;
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
        //:   'bslma::Allocator::size_type'.
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

        typedef StdTestAllocator<char>::size_type bsize;

        StdTestAllocator<char> X;
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

        Obj sta;

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
        //: 1 Create a 'StdTestAllocator' object parameterized on a test type.
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
        Grd stag(&oa);

        StdTestAllocator<TestType> sta;

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
        //:   appropriate delegate allocator.
        //:
        //: 2 The 'deallocate' method forwards the deallocation requests to the
        //:   appropriate delegate allocator.
        //
        // Plan:
        //: 1 Create a 'bslma::Allocator' object and install it as the delegate
        //:   allocator for 'StdTestAllocator'.
        //:
        //: 2 Create a new 'StdTestAllocator' object and invoke the 'allocate'
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
        Grd stag(&oa);

        Obj sta;
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
        //: 2 'size_type' is 'unsigned int' while 'difference_type' is 'int'.
        //:
        //: 3 'rebind<BDE_OTHER_TYPE>::other' defines a template instance for
        //:   'StdTestAllocator' parameterized on the 'BDE_OTHER_TYPE' type.
        //
        // Plan:
        //: 1 Define three aliases for 'StdTestAllocator' parameterized on
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

        typedef StdTestAllocator<int>   AI;
        typedef StdTestAllocator<float> AF;
        typedef StdTestAllocator<void>  AV;

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT((bsl::is_same<AI::size_type, size_t>::value));
            ASSERT((bsl::is_same<AV::size_type, size_t>::value));
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT((bsl::is_same<AI::difference_type, ptrdiff_t>::value));
            ASSERT((bsl::is_same<AV::difference_type, ptrdiff_t>::value));
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
            ASSERT((bsl::is_same<AV::pointer, void*>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
            ASSERT((bsl::is_same<AV::const_pointer, const void*>::value));
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
            ASSERT((bsl::is_same<AV::value_type, void>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AI::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AV::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AV::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AV::rebind<void >::other, AV>::value));
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
        //   Since 'StdTestAllocator' doesn't hold any state and provides the
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
        //: 2 Create two sets of 'StdTestAllocator' objects (parameterized on
        //:   void and int) and assign a non-modifiable reference of one to the
        //:   other.  (C-2)
        //
        // Testing:
        //   StdTestAllocator& operator=(const StdTestAllocator& rhs);
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

        Obj mW, mX; const Obj& X = mX;
        mW = X;

        ObjV mY, mZ; const ObjV& Z = mZ;
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
        //   Since 'StdTestAllocator' doesn't hold any state and provides the
        //   compiler supplied copy constructor, there are no concerns beyond
        //   the signatures are correctly defined.
        //
        // Concerns:
        //: 1 A 'StdTestAllocator' object can be copy constructed from a const
        //:   reference of another object of the same type.
        //:
        //: 2 A 'StdTestAllocator' object can be copy constructed from a const
        //:   reference of another object of the 'StdTestAllocator' template
        //:   instance parameterized on a different type.
        //
        // Plan:
        //: 1 Create a 'StdTestAllocator' object.  Use a const reference of the
        //:   object to copy construct another object of the same type.  (C-1)
        //:
        //: 2 Use the object created in P-1 to copy construct an object of the
        //:   'StdTestAllocator' template instant parameterized on a different
        //:   type.  (C-2)
        //
        // Testing:
        //   StdTestAllocator(const StdTestAllocator& original);
        //   StdTestAllocator(const StdTestAllocator<OTHER>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        Obj mX; const Obj& X = mX;
        Obj mY(X);
        (void) mY;

        ObjF Z(X);
        ObjV ZZ(X);

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
        //: 2 'operator!=' always return false, even for objects of different
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
        //: 2 Create a few 'StdTestAllocator' objects of different template
        //:   instances.  Verify invoking 'operator==' returns true and
        //:   invoking 'operator!=' returns false.  (C-1..2)
        //
        // Testing:
        //   bool operator==(const StdTestAllocator<TYPE>& lhs, rhs);
        //   bool operator!=(const StdTestAllocator<TYPE>& lhs, rhs);
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

        const Obj X;
        const Obj Y;
        const Obj Z;

        const ObjF A;
        const ObjV B;


        ASSERT(X == X);
        ASSERT(X == Y);
        ASSERT(!(X != Y));
        ASSERT(X == Z);
        ASSERT(!(X != Z));

        ASSERT(X == A);
        ASSERT(A == B);
        ASSERT(!(X != B));

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
        // 'StdTestAllocatorConfigurationGuard'
        //
        // Concerns:
        //: 1 The 'StdTestAllocatorConfigurationGuard' constructor changes the
        //:   delegate allocator.
        //:
        //: 2 The 'StdTestAllocatorConfigurationGuard' destructor restores the
        //:   original delegate allocator.
        //
        // Plan:
        //: 1 In a code block, create a 'bslma::TestAllocator' object, and
        //:   create a 'StdTestAllocatorConfigurationGuard' object passing in a
        //:   pointer to the just created allocator.  Verify that
        //:   'StdTestAllocatorConfiguration::delegateAllocator' returns the
        //:   pointer passed in the constructor.  (C-1)
        //:
        //: 2 Out side of the code block created in P-1, verify that
        //:   'StdTestAllocatorConfiguration::delegateAllocator' returns the
        //:   original delegate allocator.  (C-2)
        //
        // Testing:
        //   StdTestAllocatorConfigurationGuard(bslma::Allocator *temporary);
        //   ~StdTestAllocatorConfigurationGuard();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'StdTestAllocatorConfigurationGuard'"
                            "\n==============================");

        bslma::Allocator *original = Conf::delegateAllocator();
        {
            bslma::TestAllocator scratch("object", veryVeryVeryVerbose);
            Grd sag(&scratch);
            ASSERT(&scratch == Conf::delegateAllocator());
        }

        ASSERT(original == Conf::delegateAllocator());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'setDelegateAllocatorRaw' and 'delegateAllocator'
        //
        // Concerns:
        //: 1 The 'StdTestAllocatorConfiguration::setDelegateAllocatorRaw'
        //:   class method set the allocator to which 'StdTestAllocator'
        //:   delegates.
        //:
        //: 2 The 'StdTestAllocatorConfiguration::delegateAllocator' class
        //:   method returns the current delegate allocator.
        //
        // Plan:
        //: 1 Verify that, by default, 'delegateAllocator' returns a pointer
        //:   to the 'bslma::NewDeleteAllocator' singleton.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object and invoke
        //:   'setDelegateAllocatorRaw' passing in a pointer to the just
        //:   created allocator.  Verify that 'delegateAllocator' returns the
        //:   pointer passed in the previous call.  (C-1)
        //:
        //: 3 Create a 'StdTestAllocator' object and use the (as yet unproven)
        //:   'allocate' method allocates from the just test allocator created
        //:   by P-3.  (C-1)
        //
        // Testing:
        //   void setDelegateAllocatorRaw(bslma::Allocator *basicAllocator);
        //   bslma::Allocator* delegateAllocator();
        //   StdTestAllocator();
        //   ~StdTestAllocator();
        // --------------------------------------------------------------------

          if (verbose)
              printf("\n'setDelegateAllocatorRaw' and 'delegateAllocator'"
                     "\n=================================================\n");

          ASSERT(&bslma::NewDeleteAllocator::singleton() ==
                                                    Conf::delegateAllocator());

          bslma::TestAllocator oa("object", veryVeryVeryVerbose);
          StdTestAllocatorConfiguration::setDelegateAllocatorRaw(&oa);

          ASSERT(&oa == StdTestAllocatorConfiguration::delegateAllocator());

          Obj mX;
          int* ptr = mX.allocate(2);
          ASSERT(sizeof(int)*2 == oa.numBytesInUse());

          mX.deallocate(ptr);
          ASSERT(0 == oa.numBytesInUse());

          StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                      &bslma::NewDeleteAllocator::singleton());

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
            Grd sag(&oa);
            ASSERT(Conf::delegateAllocator() == &oa);

            StdTestAllocator<int> sa;
            int *ptr = sa.allocate(2);
            ASSERT(oa.numBytesInUse() == 2*sizeof(int));
            sa.deallocate(ptr);
            ASSERT(oa.numBytesInUse() == 0);
        }

        ASSERT(&bslma::NewDeleteAllocator::singleton() ==
                                                    Conf::delegateAllocator());

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
