// bslstl_equalto.t.cpp                                               -*-C++-*-

#include <bslstl_equalto.h>

#include <bslalg_hastrait.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 3] operator()(const VALUE_TYPE&, const VALUE_TYPE&) const
// [ 2] equal_to()
// [ 2] equal_to(const equal_to)
// [ 2] ~equal_to()
// [ 2] equal_to& operator=(const equal_to&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 4] Standard typedefs
// [ 5] Bitwise-movable trait
// [ 5] IsPod trait
// [ 6] QoI: Is an empty type

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
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

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

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
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // QoI: Is an empty type
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 class 'equal_to' does not increase the size of an
        //:   object when used as a base class.
        //
        // Plan:
        //: 1 Define a non-empty class with no padding, 'TwoInts'.
        //:
        //: 2 Assert that 'TwoInts has the expected size of 8 bytes.
        //:
        //: 3 Create a class, 'DerivedInts', with identical structure to
        //:   'TwoInts' but derived from 'equal_to'.
        //:
        //: 4 Assert that both classes have the same size.
        //:
        //: 5 Create a class, 'IntsWithMember', with identical structure to
        //:   'TwoInts' and an 'equal_to' additional data member.
        //:
        //: 6 Assert that 'IntsWithMember' is larger than the other two
        //:   classes.
        //
        // Testing:
        //   QoI: Support for empty base optiization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QoI: Is an empty type"
                            "\n=============================\n");

        typedef int TYPE;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedInts : equal_to<TYPE> {
            int a;
            int b;
        };

        struct IntsWithMember {
            equal_to<TYPE> dummy;
            int              a;
            int              b;
        };

        ASSERT(8 == sizeof(TwoInts));
        ASSERT(8 == sizeof(DerivedInts));
        ASSERT(8 < sizeof(IntsWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BDE TYPE TRAITS
        //   The functor is an empty POD, and should have the appropriate BDE
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is bitwise copyable.
        //: 2 The class is bitwise moveable.
        //: 3 The class has the trivial default constructor trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1..3)
        //
        // Testing:
        //   BDE Traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TRAITS"
                            "\n==================\n");

        typedef int TYPE;

        ASSERT((bslalg::HasTrait<equal_to<TYPE>,
                                 bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT((bslalg::HasTrait<equal_to<TYPE>,
                                 bslalg::TypeTraitBitwiseCopyable>::VALUE));
        ASSERT((bslalg::HasTrait<equal_to<TYPE>,
                       bslalg::TypeTraitHasTrivialDefaultConstructor>::VALUE));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STANDARD TYPEDEFS
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function.
        //
        // Concerns:
        //: 1 The typedef 'first_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 2 The typedef 'second_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 3 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bool'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bslmf::IsSame'. (C-1..3)
        //
        // Testing:
        //   typedef first_arguent_type
        //   typedef second_arguent_type
        //   typedef result_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEFS"
                            "\n=========================\n");

        typedef int TYPE;

        ASSERT((bslmf::IsSame<bool, equal_to<TYPE>::result_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE,
                              equal_to<TYPE>::first_argument_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE,
                              equal_to<TYPE>::second_argument_type>::VALUE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function, ().
        //
        // Concerns:
        //: 1 Objects of type 'equal_to' can be invokes as a binary
        //:   predicate returning 'bool' and taking two 'const char *'
        //:   arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments have the same string value.
        //:
        //: 4 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1
        //: 2
        //: 3
        //
        // Testing:
        //   operator()(const char*, const char *) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION CALL OPERATOR"
                            "\n======================\n");

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef const char *TYPE;

        bslma_TestAllocator         da("default", veryVeryVeryVerbose);
        bslma_DefaultAllocatorGuard dag(&da);

        static const struct {
            int         d_line;
            const char *d_value;
        } DATA[] = {
            // LINE    VALUE
            {  L_,     ""   },
            {  L_,     "a"  },
            {  L_,     "A"  },
            {  L_,     "1"  },
            {  L_,     "AZ" },
            {  L_,     "Aa" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const equal_to<TYPE> compare = equal_to<TYPE>();

        for (int i = 0; i != NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *LHS      = DATA[i].d_value;
            for (int j = 0; j != NUM_DATA; ++j) {
                const char *RHS      = DATA[j].d_value;
                const bool  EXPECTED = i == j;

                LOOP_ASSERT(LINE, compare(LHS, RHS) == EXPECTED);
            }
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // IMPLICITLY DEFINED CONSTRUCTORS, DESTRUCTOR AND ASSIGNMENT OPERATOR
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile, and
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //: 2 Objects can be created using the copy constructor.
        //: 3 The copy constructor is not declared as explicit.
        //: 4 Objects can be assigned to from constant objects.
        //: 5 Assignments operations can be chained.
        //: 6 Objects can be destroyed.
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-7)
        //:
        //: 2 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const equal_to'
        //:   object. (C-1)
        //:
        //: 3 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'equal_to' from the first. (C-2,3)
        //:
        //: 4 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 5 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 6 Verify the destructor is publicly accessible by allowing the two
        //:   'equal_to' object to leave scope and be
        //:    destroyed. (C-6)
        //
        // Testing:
        //   equal_to()
        //   equal_to(const equal_to)
        //   ~equal_to()
        //   equal_to& operator=(const equal_to&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nIMPLICITLY DEFINED OPERATIONS"
                            "\n=============================\n");

        typedef int TYPE;

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        bslma_TestAllocator         da("default", veryVeryVeryVerbose);
        bslma_DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Value initialization\n");
        const equal_to<TYPE> obj1 = equal_to<TYPE>();


        if (verbose) printf("Copy initialization\n");
        equal_to<TYPE> obj2 = obj1;

        if (verbose) printf("Copy assignment\n");
        obj2 = obj1;
        obj2 = obj2 = obj1;


        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 1 Create an object 'compare' using the default ctor.
        //:
        //: 2 Call the 'compare' functor with two 'char' literals in lexical
        //:   order.
        //:
        //: 3 Call the 'compare' functor with two 'char' literals in reverse
        //:   lexical order.
        //:
        //: 4 Call the 'compare' functor with two identical 'char' literals.
        //:
        //: 5 Repeat steps 1-4 for 'equal_to<const int>' using 'int' literals.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            equal_to<char> compare;
            ASSERT( compare('A', 'A'));
            ASSERT(!compare('A', 'Z'));
            ASSERT( compare('Z', 'Z'));
        }

        {
            equal_to<const int> compare;
            ASSERT( compare(0, 0));
            ASSERT(!compare(1, 0));
            ASSERT( compare(1, 1));
        }

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
