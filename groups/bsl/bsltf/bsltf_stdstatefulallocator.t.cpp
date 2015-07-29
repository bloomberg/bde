// bsltf_stdstatefulallocator.t.cpp                                   -*-C++-*-
#include <bsltf_stdstatefulallocator.h>

#include <bsltf_simpletesttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <new>

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
// TBD This text needs a total rewrite
//
// The component under test implements a value-semantic type,
// 'StdStatefulAllocator', a utility, 'StdStatefulAllocatorConfiguration', and
// a mechanism, 'StdStatefulAllocatorConfigurationGuard'.
// 'StdStatefulAllocator' holds no internal state and delegate its operations
// to a static 'bslma_Allocator' object referred by
// 'StdStatefulAllocatorConfiguration', which provides static
// methods to access and manipulate the static pointer to that delegate
// allocator.  'StdStatefulAllocatorConfigurationGuard' provides a scoped guard
// that temporarily replace the delegate allocator with a user specified
// allocator.
//
// The fact that 'StdStatefulAllocator' doesn't hold an internal state means
// that
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
// [13] size_type;
// [15] difference_type;
// [15] pointer;
// [15] const_pointer;
// [15] value_type;
// [15] rebind<U>::other;
//*[14] propagate_on_container_copy_assignment
//*[14] propagate_on_container_move_assignment
//*[14] propagate_on_container_swap
//
// CREATORS
// [ 2] StdStatefulAllocator(bslma::allocator *);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator& original);
// [ 7] StdStatefulAllocator(const StdStatefulAllocator<OTHER>&);
// [ 2] ~StdStatefulAllocator();
//
// MANIPULATORS
// [ 9] StdStatefulAllocator& operator=(const StdStatefulAllocator& rhs);
// [11] pointer allocate(size_type numElements, const void *hint = 0);
// [11] void deallocate(pointer address, size_type numElements);
//
// ACCESSORS
// [ 4] bslma::TestAllocator *testAllocator() const;
//*[12] StdStatefulAllocator select_on_container_copy_construction() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
// [ 6] bool operator!=(const StdStatefulAllocator<TYPE,B,B,B,B>& lhs, rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLE
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

typedef StdStatefulAllocator<int>              Obj;
typedef StdStatefulAllocator<float>            ObjF;

typedef StdStatefulAllocator<int, false, false, false, false>   ObjI0;
typedef StdStatefulAllocator<int, false, false, false, true>    ObjI1;
typedef StdStatefulAllocator<int, false, false, true,  false>   ObjI2;
typedef StdStatefulAllocator<int, false, false, true,  true>    ObjI3;
typedef StdStatefulAllocator<int, false, true,  false, false>   ObjI4;
typedef StdStatefulAllocator<int, false, true,  false, true>    ObjI5;
typedef StdStatefulAllocator<int, false, true,  true,  false>   ObjI6;
typedef StdStatefulAllocator<int, false, true,  true,  true>    ObjI7;
typedef StdStatefulAllocator<int, true,  false, false, false>   ObjI8;
typedef StdStatefulAllocator<int, true,  false, false, true>    ObjI9;
typedef StdStatefulAllocator<int, true,  false, true,  false>   ObjI10;
typedef StdStatefulAllocator<int, true,  false, true,  true>    ObjI11;
typedef StdStatefulAllocator<int, true,  true,  false, false>   ObjI12;
typedef StdStatefulAllocator<int, true,  true,  false, true>    ObjI13;
typedef StdStatefulAllocator<int, true,  true,  true,  false>   ObjI14;
typedef StdStatefulAllocator<int, true,  true,  true,  true>    ObjI15;

typedef StdStatefulAllocator<float, false, false, false, false> ObjF0;
typedef StdStatefulAllocator<float, false, false, false, true>  ObjF1;
typedef StdStatefulAllocator<float, false, false, true,  false> ObjF2;
typedef StdStatefulAllocator<float, false, false, true,  true>  ObjF3;
typedef StdStatefulAllocator<float, false, true,  false, false> ObjF4;
typedef StdStatefulAllocator<float, false, true,  false, true>  ObjF5;
typedef StdStatefulAllocator<float, false, true,  true,  false> ObjF6;
typedef StdStatefulAllocator<float, false, true,  true,  true>  ObjF7;
typedef StdStatefulAllocator<float, true,  false, false, false> ObjF8;
typedef StdStatefulAllocator<float, true,  false, false, true>  ObjF9;
typedef StdStatefulAllocator<float, true,  false, true,  false> ObjF10;
typedef StdStatefulAllocator<float, true,  false, true,  true>  ObjF11;
typedef StdStatefulAllocator<float, true,  true,  false, false> ObjF12;
typedef StdStatefulAllocator<float, true,  true,  false, true>  ObjF13;
typedef StdStatefulAllocator<float, true,  true,  true,  false> ObjF14;
typedef StdStatefulAllocator<float, true,  true,  true,  true>  ObjF15;

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
// First we define a simple container type intended to be used with a C++11
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
    new((void *)d_object_p) TYPE(object);
}

template <class TYPE, class ALLOCATOR>
MyContainer<TYPE, ALLOCATOR>::~MyContainer()
{
    d_object_p->~TYPE();
    d_allocator.deallocate(d_object_p, 1);
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
// Now, we use 'StdStatefulAllocator' to implement a simple test for
// 'MyContainer' to verify it correctly uses a parameterized allocator using
// only the C++03 standard methods:
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
        // SPURIOUS NESTED TYPES
        //   There are a number of frequently encountered type aliases that are
        //   often defined by an allocator type.  For a minimal C++11 allocator
        //   these will be supplied automatically by the 'allocator_traits'
        //   template and so should *not* be defined for our minimal allocator.
        //   However, the current 'bsl' implementation of 'allocator_traits'
        //   does not perform the necessary template metaprogramming to deduce
        //   these aliases if missing, so our initial implementation must also
        //   provide these names.  Once we have a more complete implementation
        //   of 'allocator_traits', this test will instead confirm that these
        //   popular type aliases do *not* exist for our template.
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
        //   rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nSPURIOUS NESTED TYPES"
                            "\n=====================\n");

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

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // PROPAGATION TRAITS
        //   The copy, move and swap behavior of the allocators of C++11
        //   containers is determined by three "propagation traits" that may
        //   be defined by the allocator, but are otherwise assumed to be
        //   'false' i.e., do not propagate.  This component provides the
        //   ability to configure these traits for any given instantiation of
        //   this template.  These traits have no effect on the behavior of the
        //   allocator type itself, but exist solely to be queried by higher
        //   level components, such as containers.  If defined, they must be
        //   an alias to 'bsl::true_type', 'bsl::false_type', or a type
        //   publicly and unambiguously derived from one of these two classes.
        //
        // Concerns:
        //: 1 TBD ...
        //:
        // Plan:
        //: 1 TBD ... (C-1)
        //
        // Testing:
        //   propagate_on_container_copy_assignment
        //   propagate_on_container_move_assignment
        //   propagate_on_container_swap

        // --------------------------------------------------------------------

        if (verbose) printf("\nPROPAGATION TRAITS"
                            "\n==================\n");

        ASSERT(!ObjI0::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI1::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI2::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI3::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI4::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI5::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI6::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI7::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI8::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI9::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI10::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjI11::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI12::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI13::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI14::propagate_on_container_copy_assignment::value);
        ASSERT( ObjI15::propagate_on_container_copy_assignment::value);

        ASSERT(!ObjI0::propagate_on_container_move_assignment::value);
        ASSERT( ObjI1::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI2::propagate_on_container_move_assignment::value);
        ASSERT( ObjI3::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI4::propagate_on_container_move_assignment::value);
        ASSERT( ObjI5::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI6::propagate_on_container_move_assignment::value);
        ASSERT( ObjI7::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI8::propagate_on_container_move_assignment::value);
        ASSERT( ObjI9::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI10::propagate_on_container_move_assignment::value);
        ASSERT( ObjI11::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI12::propagate_on_container_move_assignment::value);
        ASSERT( ObjI13::propagate_on_container_move_assignment::value);
        ASSERT(!ObjI14::propagate_on_container_move_assignment::value);
        ASSERT( ObjI15::propagate_on_container_move_assignment::value);

        ASSERT(!ObjI0::propagate_on_container_swap::value);
        ASSERT(!ObjI1::propagate_on_container_swap::value);
        ASSERT( ObjI2::propagate_on_container_swap::value);
        ASSERT( ObjI3::propagate_on_container_swap::value);
        ASSERT(!ObjI4::propagate_on_container_swap::value);
        ASSERT(!ObjI5::propagate_on_container_swap::value);
        ASSERT( ObjI6::propagate_on_container_swap::value);
        ASSERT( ObjI7::propagate_on_container_swap::value);
        ASSERT(!ObjI8::propagate_on_container_swap::value);
        ASSERT(!ObjI9::propagate_on_container_swap::value);
        ASSERT( ObjI10::propagate_on_container_swap::value);
        ASSERT( ObjI11::propagate_on_container_swap::value);
        ASSERT(!ObjI12::propagate_on_container_swap::value);
        ASSERT(!ObjI13::propagate_on_container_swap::value);
        ASSERT( ObjI14::propagate_on_container_swap::value);
        ASSERT( ObjI15::propagate_on_container_swap::value);

        ASSERT(!ObjF0::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF1::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF2::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF3::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF4::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF5::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF6::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF7::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF8::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF9::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF10::propagate_on_container_copy_assignment::value);
        ASSERT(!ObjF11::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF12::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF13::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF14::propagate_on_container_copy_assignment::value);
        ASSERT( ObjF15::propagate_on_container_copy_assignment::value);

        ASSERT(!ObjF0::propagate_on_container_move_assignment::value);
        ASSERT( ObjF1::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF2::propagate_on_container_move_assignment::value);
        ASSERT( ObjF3::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF4::propagate_on_container_move_assignment::value);
        ASSERT( ObjF5::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF6::propagate_on_container_move_assignment::value);
        ASSERT( ObjF7::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF8::propagate_on_container_move_assignment::value);
        ASSERT( ObjF9::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF10::propagate_on_container_move_assignment::value);
        ASSERT( ObjF11::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF12::propagate_on_container_move_assignment::value);
        ASSERT( ObjF13::propagate_on_container_move_assignment::value);
        ASSERT(!ObjF14::propagate_on_container_move_assignment::value);
        ASSERT( ObjF15::propagate_on_container_move_assignment::value);

        ASSERT(!ObjF0::propagate_on_container_swap::value);
        ASSERT(!ObjF1::propagate_on_container_swap::value);
        ASSERT( ObjF2::propagate_on_container_swap::value);
        ASSERT( ObjF3::propagate_on_container_swap::value);
        ASSERT(!ObjF4::propagate_on_container_swap::value);
        ASSERT(!ObjF5::propagate_on_container_swap::value);
        ASSERT( ObjF6::propagate_on_container_swap::value);
        ASSERT( ObjF7::propagate_on_container_swap::value);
        ASSERT(!ObjF8::propagate_on_container_swap::value);
        ASSERT(!ObjF9::propagate_on_container_swap::value);
        ASSERT( ObjF10::propagate_on_container_swap::value);
        ASSERT( ObjF11::propagate_on_container_swap::value);
        ASSERT(!ObjF12::propagate_on_container_swap::value);
        ASSERT(!ObjF13::propagate_on_container_swap::value);
        ASSERT( ObjF14::propagate_on_container_swap::value);
        ASSERT( ObjF15::propagate_on_container_swap::value);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // NESTED TYPES
        //
        // Concerns:
        //: 1 The 'typedef' aliases defined in this component are the minimal
        //:   set specified by the C++11 standard for types satisfying the
        //:   'Allocator' requirements.
        //:
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
        //   value_type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nNESTED TYPES"
                            "\n============\n");

        typedef StdStatefulAllocator<int>   AI;
        typedef StdStatefulAllocator<float> AF;

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'select_on_container_copy_construction' AND 'deallocate'
        //
        // Concerns:
        //: 1 TBD ...
        //
        // Plan:
        //: 1 TBD ... (C-1)
        //
        // Testing:
        //   StdStatefulAllocator select_on_container_copy_construction() const
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\nTesting 'select_on_container_copy_construction'"
                        "\n===============================================\n");

        {
            bslma::TestAllocator da("default allocator");

            bslma::TestAllocator ta[16];

            bslma::DefaultAllocatorGuard dag(&da);

            {
                const ObjI0 source(&ta[0]);
                ObjI0 copy = source.select_on_container_copy_construction();

                ASSERT(source != copy);
                ASSERT(copy.testAllocator() == &da);
            }

            {
                ObjF0 source(&ta[0]);
                ObjF0 copy = source.select_on_container_copy_construction();

                ASSERT(source != copy);
                ASSERT(copy.testAllocator() == &da);
            }

            {
                ObjI1 source(&ta[1]);
                ObjI1 copy = source.select_on_container_copy_construction();

                ASSERT(source != copy);
                ASSERT(copy.testAllocator() == &da);
            }

            {
                const ObjF1 source(&ta[1]);
                ObjF1 copy = source.select_on_container_copy_construction();

                ASSERT(source != copy);
                ASSERT(copy.testAllocator() == &da);
            }

            {
                ObjI8 source(&ta[8]);
                ObjI8 copy = source.select_on_container_copy_construction();

                ASSERT(source == copy);
                ASSERT(copy.testAllocator() != &da);
            }

            {
                const ObjF8 source(&ta[8]);
                ObjF8 copy = source.select_on_container_copy_construction();

                ASSERT(source == copy);
                ASSERT(copy.testAllocator() != &da);
            }

            {
                const ObjI15 source(&ta[15]);
                ObjI15 copy = source.select_on_container_copy_construction();

                ASSERT(source == copy);
                ASSERT(copy.testAllocator() != &da);
            }

            {
                ObjF15 source(&ta[15]);
                ObjF15 copy = source.select_on_container_copy_construction();

                ASSERT(source == copy);
                ASSERT(copy.testAllocator() != &da);
            }
        }

      } break;
      case 11: {
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
        //: 1 Create a 'bslma_Allocator' object and install it as the delegate
        //:   allocator for 'StdStatefulAllocator'.
        //:
        //: 2 Create a new 'StdStatefulAllocator' object and invoke the
        //:   'allocate' method.  Verify that the correct amount of memory has
        //:    been allocated from the delegate allocator.  (C-1)
        //:
        //: 3 Invoke the 'deallocate' method and verify that the correct amount
        //:   of memory has been deallocated from the delegate allocator. (C-2)
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

        sta.deallocate(ptr, 1);

        ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), sizeof(int) == oa.numBytesTotal());

        ptr = sta.allocate(4);

        ASSERTV(oa.numBytesInUse(), 4 * sizeof(int) == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), 5 * sizeof(int) == oa.numBytesTotal());

        sta.deallocate(ptr, 4);

        ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        ASSERTV(oa.numBytesTotal(), 5 * sizeof(int) == oa.numBytesTotal());

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
        //   Since 'StdStatefulAllocator' doesn't hold any state and provides
        //   the compiler supplied assignment operator, there are no concerns
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
        //: 2 Create two sets of 'StdStatefulAllocator' objects (parameterized
        //:   on 'void' and 'int') and assign a non-modifiable reference of one
        //:   to the other.  (C-2)
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

            (void) operatorAssignment;  // quash potential compiler warning
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
        //   Since 'StdStatefulAllocator' doesn't hold any state and provides
        //   the compiler supplied copy constructor, there are no concerns
        //   beyond the signatures are correctly defined.
        //
        // Concerns:
        //: 1 A 'StdStatefulAllocator' object can be copy constructed from a
        //:   const reference of another object of the same type.
        //:
        //: 2 A 'StdStatefulAllocator' object can be copy constructed from a
        //:   const reference of another object of the 'StdStatefulAllocator'
        //:   template instance parameterized on a different type.
        //
        // Plan:
        //: 1 Create a 'StdStatefulAllocator' object.  Use a const reference of
        //:   the object to copy construct another object of the same type.
        //:   (C-1)
        //:
        //: 2 Use the object created in P-1 to copy construct an object of the
        //:   'StdStatefulAllocator' template instant parameterized on a
        //:   different type.  (C-2)
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

            (void) operatorEq;  // quash potential compiler warnings
            (void) operatorNe;
        }

        bslma::TestAllocator allocX("X");
        bslma::TestAllocator allocY("Y");

        const Obj X(&allocX);
        const Obj Y(&allocY);

        ASSERT(X == X);
        ASSERT(X != Y);
        ASSERT(!(X == Y));
        ASSERT(!(X != X));

        ASSERT(Y == Y);
        ASSERT(Y != X);
        ASSERT(!(Y == X));
        ASSERT(!(Y != Y));

        const ObjF A(&allocX);
        const ObjF B(&allocY);

        ASSERT(A == A);
        ASSERT(A != B);
        ASSERT(!(A == B));
        ASSERT(!(A != A));

        ASSERT(B == B);
        ASSERT(B != A);
        ASSERT(!(B == A));
        ASSERT(!(B != B));


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
        //: 1 Use the value constructor, create an object having the expected
        //:   attribute values.  Verify that the accessor for the
        //:   'testAllocator' attribute invoked on a reference providing
        //:   non-modifiable access to the object return the expected value.
        //:   (C-1, 2)
        //:
        // Testing:
        //   bslma::TestAllocator *testAllocator() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS"
                   "\n===============\n");

        bslma::TestAllocator allocX("X");
        bslma::TestAllocator allocY("Y");

        const Obj X(&allocX);
        const Obj Y(&allocY);

        ASSERTV(&allocX, X.testAllocator(), &allocX == X.testAllocator());
        ASSERTV(&allocY, Y.testAllocator(), &allocY == Y.testAllocator());

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
                            "\n==========================\n");
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Value constructor and basic manipulators (if any)
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
        //: 3 Create a 'StdStatefulAllocator' object and use the (as yet
        //:   unproven) 'allocate' method allocates from the just test
        //:   allocator created by P-3.  (C-1)
        //
        // Testing:
        //   StdStatefulAllocator(bslma::TestAllocator *);
        //   ~StdStatefulAllocator();
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nValue constructor and basic manipulators (if any)"
                   "\n=================================================\n");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);
        int* ptr = mX.allocate(2);
        ASSERT(sizeof(int)*2 == oa.numBytesInUse());

        mX.deallocate(ptr, 2);
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
            sa.deallocate(ptr, 2);
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
