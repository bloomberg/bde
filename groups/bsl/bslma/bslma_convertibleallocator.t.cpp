// bslma_convertibleallocator.t.cpp                                   -*-C++-*-

#include "bslma_convertibleallocator.h"

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -MN01   // Class data members must be private
#pragma bde_verify -FABC01 // Function not in alphabetical order
#endif

using std::printf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component defines a single class, 'ConvertibleAllocator', which is an
// in-core value-semantic type supporting copy construction and equality
// comparison but but not assignment.  Because it is derived from
// 'bsl::allocator<char>', 'ConvertibleAllocator' but does not override any of
// its methods, it is not necessary to test any of the base-class
// functionality.  All that is left, then, is to test the new, derived-class,
// functionality: constructors, equality comparison, and conversions to other
// types.  All of the derived-class properties of 'ConvertibleAllocator' boil
// down to copying the underlying allocator resource or else testing it for
// equality.  Thus the tests simply perform these operations and verify that
// the (inherited) 'mechanism' method returns the expected value.
// ----------------------------------------------------------------------------
// [ 2] ConvertibleAllocator(bslma::Allocator *)
// [ 2] ConvertibleAllocator(const bsl::allocator<char>&)
// [ 2] ConvertibleAllocator(bslmf::MovableRef<bsl::allocator<char> >)
// [ 6] ConvertibleAllocator(const ConvertibleAllocator&)
// [ 3] CONVERSION TO 'bsl::allocator'
// [ 5] bool operator==(const CvtableAlloc&, const CvtableAlloc&)
// [ 5] bool operator!=(const CvtableAlloc&, const CvtableAlloc&)
// [ 5] bool operator==(const CvtableAlloc&, const CvtableAlloc&)
// [ 5] bool operator!=(const CvtableAlloc&, const CvtableAlloc&)
// [ 5] bool operator==(const CvtableAlloc&, const bsl::allocator<TYPE>&)
// [ 5] bool operator==(const bsl::allocator<TYPE>&, const CvtableAlloc&)
// [ 5] bool operator!=(const CvtableAlloc&, const bsl::allocator<TYPE>&)
// [ 5] bool operator!=(const bsl::allocator<TYPE>&, const CvtableAlloc&)
// [ 5] bool operator==(const ConvertibleAllocator&, Allocator *)
// [ 5] bool operator==(Allocator *, const ConvertibleAllocator&)
// [ 5] bool operator!=(const ConvertibleAllocator&, Allocator *)
// [ 5] bool operator!=(Allocator *, const ConvertibleAllocator&)
// [ 4] operator Allocator *() const
// [ 7] UsesBslmaAllocator<ConvertibleAllocator>
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//            GLOBAL TYPEDEFS, CONSTANTS, AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bslma::Allocator *convertToConvertibleAllocator(bslma::ConvertibleAllocator a)
    // Return the mechanism for the specified 'a' allocator.
{
    return a.mechanism();
}

bslma::Allocator *
convertToConvertibleAllocatorRef(const bslma::ConvertibleAllocator& a)
    // Return the mechanism for the specified 'a' allocator.
{
    return a.mechanism();
}

template <class TYPE>
bslma::Allocator *convertToBslAllocator(bsl::allocator<TYPE> a)
    // Return the mechanism for the specified 'a' allocator.
{
    return a.mechanism();
}

template <class TYPE>
bslma::Allocator *convertToBslAllocatorRef(const bsl::allocator<TYPE>& a)
    // Return the mechanism for the specified 'a' allocator.
{
    return a.mechanism();
}

bslma::Allocator *convertToBslmaAllocator(bslma::Allocator *a)
    // Return specified 'a' allocator pointer.
{
    return a;
}

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Example 1: A Constructor Supporting Both *bsl-AA* and *legacy-AA* Models
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'ConvertibleAllocator' can be used to construct
// an Allocator-Aware (AA) object without regard to whether it is *legacy-AA*
// (using 'bslma::Allocator *') or *bsl-AA* (using 'bsl::allocator').  Consider
// a *bsl-AA* class, 'Wrapper', that wraps an object of template-parameter
// type, 'TYPE'.  First, we define the data members:
//..
    template <class TYPE>
    class Wrapper {
        // Wrap an object of type 'TYPE'.

        // DATA
        bsl::allocator<char> d_allocator;
        TYPE                 d_object;

      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;
//..
// Next, we define the constructors.  The constructors for 'Wrapper' would all
// take an optional 'allocator_type' argument, but the 'd_object' member might
// require a 'bslma::Allocator *' argument.  By converting the allocator to
// 'ConvertibleAllocator', we can be agnostic to that issue:
//..
        // CREATORS
        explicit Wrapper(const allocator_type& alloc = allocator_type())
            : d_allocator(alloc)
            , d_object(bslma::ConvertibleAllocator(alloc))
            { }

        Wrapper(const Wrapper&, const allocator_type& = allocator_type());

//..
// Next, we Finish up our class by creating accessors to get the allocator and
// wrapped object:
//..
        // ACCESSORS
        const allocator_type get_allocator() const { return d_allocator; }
            // Return the allocator used to construct this object.

        const TYPE& value() const { return d_object; }
    };
//..
// Now, to see the effect of the constructor, we'll use a pair of simple AA
// classes, 'SampleBslAAType' and 'SampleLegcyAAType' that do nothing more than
// hold an allocator:
//..
    class SampleBslAAType {
        // Sample AA class that adheres to the bsl-AA interface.

        // DATA
        bsl::allocator<char> d_allocator;

      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;

        // CREATORS
        explicit SampleBslAAType(const allocator_type& alloc =allocator_type())
            : d_allocator(alloc) { }

        // ...

        // ACCESSORS
        allocator_type get_allocator() const { return d_allocator; }
    };

    class SampleLegacyAAType {
        // Sample AA class that adheres to the legacy-AA interface.

        // DATA
        bslma::Allocator *d_allocator_p;

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(SampleLegacyAAType,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        explicit SampleLegacyAAType(bslma::Allocator *basicAllocator = 0)
            : d_allocator_p(bslma::Default::allocator(basicAllocator)) { }

        // ...

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }
    };
//..
// Finally, in our main program, create an allocator and pass it to a couple of
// 'Wrapper' objects, one instantiated with 'SampleBslAAType' and the other
// instantiated with our 'SampleLegacyAAType'.  We verify that both were
// constructed appropriately, with the supplied allocator being used by the
// wrapped object:
//..
    void usageExample1()
    {
        bslma::TestAllocator ta;
        bsl::allocator<char> alloc(&ta);

        Wrapper<SampleBslAAType> w1(alloc);
        ASSERT(alloc == w1.get_allocator());
        ASSERT(alloc == w1.value().get_allocator());

        Wrapper<SampleLegacyAAType> w2(alloc);
        ASSERT(alloc == w2.get_allocator());
        ASSERT(&ta   == w2.value().allocator());
    }
//..
// Note that a more complete example would use metaprogramming to choose the
// constructor arguments for the wrapped object so that non-AA types could be
// supported as well as types for which the allocator argument in preceded by
// an argument of type 'bsl::allocator_arg_t' (see 'bslma::ConstructionUtil').

}  // close unnamed namespace

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

    typedef bslmf::MovableRefUtil MoveUtil;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //:   'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // 'UsesBslmaAllocator<ConvertibleAllocator>' TRAIT
        //
        // Concerns:
        //: 1 That 'UsesBslmaAllocator<ConvertibleAllocator>::value' is 'false'
        //:   even though 'ConvertibleAllocator' is implicitly convertible from
        //:   'bslma::Allocator *'.
        //
        // Plan:
        //: 1 Verify that 'UsesBslmaAllocator<ConvertibleAllocator>::value' is
        //:   'false'.
        //
        // Testing:
        //      UsesBslmaAllocator<ConvertibleAllocator>
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'UsesBslmaAllocator<ConvertibleAllocator>' TRAIT"
                   "\n================================================\n");

        ASSERT(!bslma::UsesBslmaAllocator<bslma::ConvertibleAllocator>::value);

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 A copy-constructed 'ConvertibleAllocator' compares equal to the
        //:   object it was copied from.
        //: 2 The original object is unchanged.
        //: 3 Concerns 1 and 2 apply whether the original object is passed by
        //:   lvalue, rvalue (prvalue), or 'MovableRef' (xvalue).
        //
        // Plan:
        //: 1 Create objects of type 'ConvertibleAllocator' using both the
        //:   'bslma::Allocator *' constructor.
        //: 2 Create copies of the object from step 1 using the copy
        //:   constructor.  Verify that the compare equal to the original and
        //:   not equal to the other 'ConvertibleAllocator'.  (C-1)
        //: 3 In step 2, verify that the original allocator's mechanism is the
        //:   same as that with which it was constructed.  (C-2)
        //: 4 Repeat steps 2 and 3 with prvalue and xvalue arguments.
        //
        // Testing
        //      ConvertibleAllocator(const ConvertibleAllocator&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        using namespace bslma;

        // Step 1: Create objects
        bslma::TestAllocator ta;
        ConvertibleAllocator ca1(0);   // Default allocator
        ConvertibleAllocator ca2(&ta);
        ASSERT(ca1.mechanism() == bslma::Default::defaultAllocator());
        ASSERT(ca2.mechanism() == &ta);
        ASSERT(ca1 != ca2);

        // Steps 2 and 3: Copy objects
        ConvertibleAllocator cca1(ca1);
        ASSERT(cca1 == ca1);
        ASSERT(cca1 != ca2);
        ASSERT(ca1.mechanism() == bslma::Default::defaultAllocator());

        ConvertibleAllocator cca2(ca2);
        ASSERT(cca2 == ca2);
        ASSERT(cca2 != ca1);
        ASSERT(ca2.mechanism() == &ta);

        // Step 4: Repeat with prvalues and xvalues

        // From prvalues
        ConvertibleAllocator prca1 = ConvertibleAllocator(0);
        ASSERT(prca1 == ConvertibleAllocator(0));
        ASSERT(prca1 == ca1);
        ASSERT(prca1 != ca2);

        ConvertibleAllocator prca2 = ConvertibleAllocator(&ta);
        ASSERT(prca2 == ConvertibleAllocator(&ta));
        ASSERT(prca2 == ca2);
        ASSERT(prca2 != ca1);

        // From xvalues
        ConvertibleAllocator xca1(MoveUtil::move(ca1));
        ASSERT(xca1 == ca1);
        ASSERT(xca1 != ca2);
        ASSERT(ca1.mechanism() == bslma::Default::defaultAllocator());

        ConvertibleAllocator xca2(MoveUtil::move(ca2));
        ASSERT(xca2 == ca2);
        ASSERT(xca2 != ca1);
        ASSERT(ca2.mechanism() == &ta);

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY COMPARISON
        //    Because 'ConvertibleAllocator' is convertible to and from
        //    'bsl::allocator' and 'bslma::Allocator *', there are many
        //    opportunities for ambiguities when comparing objects of this type
        //    using '==' or '!='.  This test cases probes all combinations of
        //    'ConvertibleAllocator' with the types it converts directly to and
        //    from to ensure that equality comparisons compile and produce the
        //    correct results in all cases.
        //
        // Concerns:
        //: 1 'operator==' returns 'true' when comparing 'ConvertibleAllocator'
        //:   objects when they have the same mechanism and 'false' when they
        //:   have different mechanisms.  The results are reversed for
        //:   'operator!='.
        //: 2 A 'ConvertibleAllocator' always compares equal to itself.
        //: 3 'operator==' returns 'true' when comparing a
        //:   'ConvertibleAllocator' object to a 'bsl::allocator' object when
        //:   they have the same mechanism and 'false' when they have different
        //:   mechanisms.  The results are reversed for 'operator!='.  For both
        //:   operators, the results are unaffected by the order of operands on
        //:   the left and right of the operator.
        //: 4 'operator==' returns 'true' when comparing a
        //:   'ConvertibleAllocator', 'C', to a 'bslma::Allocator *' 'P' when
        //:   'C.mechanism() == P' and 'false' otherwise.  The results are
        //:   reversed for 'operator!='.  For both operators, the results are
        //:   unaffected by the order of operands on the left and right of the
        //:   operator.
        //: 5 The above concerns all apply if one or both operands are rvalue
        //:   expressions or 'bslmf::MovableRef' (xvalue) expressions.
        //: 6 For two arguments 'A1' and 'A2' that are convertible to
        //:   'ConvertibleAllocator', but where neither is actually an lvalue
        //:   or rvalue of 'ConvertibleAllocator' type, having the
        //:   'ConvertibleAllocator' operators available does not cause any
        //:   ambiguity when invoking the expressions 'A1 == A2' or 'A1 != A2'.
        //
        // Plan:
        //: 1 Create three 'bslma::TestAllocator' objects, setting one as the
        //:   temporary default allocator and create five
        //:   'ConvertibleAllocator' objects: two using the default test
        //:   allocator via null-pointer construction, two using the second
        //:   test allocator, and one using the third test allocator.
        //: 2 Verify comparing any two of the 'ConvertibleAllocator' objects
        //:   from step 1 that are constructed with the same test allocator
        //:   yields 'true' with the equality operator ('==') and 'false' with
        //:   the inequality operator ('!=').  (C-1)
        //: 3 Verify comparing any two of the 'ConvertibleAllocator' objects
        //:   from step 1 that are constructed with the different test
        //:   allocators yields 'false' with the equality operator ('==') and
        //:   'true' with the inequality operator ('!=').  (C-1)
        //: 4 Compare a representative sample of the 'ConvertibleAllocator'
        //:   objects from step 1 with themselves and verify that they compare
        //:   equal (C-2)
        //: 5 Construct three 'bsl::allocator' objects, each using a different
        //:   test allocator from step 1.  Compare each of these allocators
        //:   against each of the different 'ConvertibleAllocator's from step 1
        //:   and verify that they compare equal when they were constructed
        //:   with the same test allocator and unequal otherwise.  Perform this
        //:   test with the 'bsl::allocator' on the left and right of the
        //:   equality and inequality operator.  (C-3)
        //: 6 Compare the address of each of the test allocators from step one
        //:   against each of the different 'ConvertibleAllocator's from step 1
        //:   and verify that they compare equal when the
        //:   'ConvertibleAllocator' was constructed with the specific test
        //:   allocator and unequal otherwise.  Perform this test with the
        //:   'bslma::TestAllocator *' on the left and right of the equality
        //:   and inequality operator.  (C-4)
        //: 7 Repeat a representative portion of steps 2, 3, 5, and 6, using a
        //:   direct-constructed object (prvalue) or the result of
        //:   'bslmf::MovableRefUtil::move' (xvalue) for one or both operands.
        //:   Verify that the results are unchanged by the presence of these
        //:   rvalues.  (C-5)
        //: 8 Invoke expressions 'A1 == A2' and 'A1 != A2' for arguments 'A1'
        //:   and 'A2' having type 'bslma::Allocator *' and 'bsl::allocator',
        //:   in every combination; verify that the expressions are unambiguous
        //:   and yield the expected result when
        //:   '<bslma_convertibleallocator.h>' has been included.
        //
        //  Testing:
        //    bool operator==(const CvtableAlloc&, const CvtableAlloc&)
        //    bool operator!=(const CvtableAlloc&, const CvtableAlloc&)
        //    bool operator==(const CvtableAlloc&, const CvtableAlloc&)
        //    bool operator!=(const CvtableAlloc&, const CvtableAlloc&)
        //    bool operator==(const CvtableAlloc&, const bsl::allocator<TYPE>&)
        //    bool operator==(const bsl::allocator<TYPE>&, const CvtableAlloc&)
        //    bool operator!=(const CvtableAlloc&, const bsl::allocator<TYPE>&)
        //    bool operator!=(const bsl::allocator<TYPE>&, const CvtableAlloc&)
        //    bool operator==(const ConvertibleAllocator&, Allocator *)
        //    bool operator==(Allocator *, const ConvertibleAllocator&)
        //    bool operator!=(const ConvertibleAllocator&, Allocator *)
        //    bool operator!=(Allocator *, const ConvertibleAllocator&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY COMPARISON"
                            "\n===================\n");

        using namespace bslma;

        // Step 1: Create test allocators and 'ConvertibleAllocator' objects
        TestAllocator         da, ta1, ta2;
        DefaultAllocatorGuard guard(&da);

        ConvertibleAllocator        CaDfltA(0), CaDfltB(0);  // mechanism = &da
        ConvertibleAllocator        CaTa1A(&ta1), CaTa1B(&ta1);
        ConvertibleAllocator        CaTa2(&ta2);
        const ConvertibleAllocator& CADFLTA = CaDfltA;
        const ConvertibleAllocator& CADFLTB = CaDfltB;
        const ConvertibleAllocator& CATA1A  = CaTa1A;
        const ConvertibleAllocator& CATA1B  = CaTa1B;
        const ConvertibleAllocator& CATA2   = CaTa2;

        ASSERT(&da == CADFLTA.mechanism() && &da == CADFLTB.mechanism());
        ASSERT(&ta1 == CATA1A.mechanism() && &ta1 == CATA1B.mechanism());
        ASSERT(&ta2 == CATA2.mechanism());

        // Step 2: Compare equal 'ConvertibleAllocator' objects
        ASSERT(  CADFLTA == CADFLTB);
        ASSERT(!(CADFLTA != CADFLTB));
        ASSERT(  CATA1A  == CATA1B);
        ASSERT(!(CATA1A  != CATA1B));

        // Step 3: Compare unequal 'ConvertibleAllocator' objects
        ASSERT(!(CADFLTA == CATA1A));
        ASSERT(  CATA1A  != CADFLTA);
        ASSERT(!(CATA2   == CATA1A));
        ASSERT(  CATA1A  != CATA2);

        // Step 4: Compare objects to self
        ASSERT(  CADFLTB == CADFLTB);
        ASSERT(!(CADFLTB != CADFLTB));
        ASSERT(  CATA2   == CATA2);
        ASSERT(!(CATA2   != CATA2));

        // Step 5: Compare with 'bsl::allocator'
        bsl::allocator<char>        bslaA;       // mechanism = &da
        bsl::allocator<int>         bslaB(&ta1);
        const bsl::allocator<char>& BSLAA = bslaA;
        const bsl::allocator<int>&  BSLAB = bslaB;

        ASSERT(BSLAA  == CADFLTA);
        ASSERT(BSLAB  != CADFLTA);
        ASSERT(CATA1A == BSLAB);
        ASSERT(CATA2  != BSLAB);

        // Step 6: Compare with 'bslma::Allocator *'
        ASSERT(CADFLTA == &da);
        ASSERT(&ta1    != CADFLTB);
        ASSERT(CATA2   == &ta2);
        ASSERT(CATA2   != &da);

        // Step 7: Repeat using rvalues
        ASSERT(CADFLTA == ConvertibleAllocator(0)    );
        ASSERT(CATA1A  != ConvertibleAllocator(0)    );
        ASSERT(CATA1A  == ConvertibleAllocator(&ta1));
        ASSERT(CATA1A  != ConvertibleAllocator(&ta2));
        ASSERT(CADFLTA == MoveUtil::move(CaDfltB));
        ASSERT(CATA1A  != MoveUtil::move(CaDfltA));
        ASSERT(CATA1A  == MoveUtil::move(CaTa1B) );
        ASSERT(CADFLTB != MoveUtil::move(CaTa1B) );

        ASSERT(ConvertibleAllocator(0)    == CADFLTA);
        ASSERT(ConvertibleAllocator(0)    != CATA1A );
        ASSERT(ConvertibleAllocator(&ta1) == CATA1A );
        ASSERT(ConvertibleAllocator(&ta2) != CATA1A );
        ASSERT(MoveUtil::move(CaDfltB)    == CADFLTA);
        ASSERT(MoveUtil::move(CaDfltA)    != CATA1A );
        ASSERT(MoveUtil::move(CaTa1B)     == CATA1A );
        ASSERT(MoveUtil::move(CaTa1B)     != CADFLTB);

        ASSERT(ConvertibleAllocator(0)    == ConvertibleAllocator(0)   );
        ASSERT(ConvertibleAllocator(&ta1) != ConvertibleAllocator(0)   );
        ASSERT(ConvertibleAllocator(&ta1) == ConvertibleAllocator(&ta1));
        ASSERT(ConvertibleAllocator(&ta2) != ConvertibleAllocator(&ta1));
        ASSERT(ConvertibleAllocator(0)    == MoveUtil::move(CaDfltA)   );
        ASSERT(ConvertibleAllocator(0)    != MoveUtil::move(CaTa1A )   );
        ASSERT(ConvertibleAllocator(&ta1) == MoveUtil::move(CaTa1A )   );
        ASSERT(ConvertibleAllocator(&ta2) != MoveUtil::move(CaTa1A )   );
        ASSERT(MoveUtil::move(CaDfltA)    == ConvertibleAllocator(0)   );
        ASSERT(MoveUtil::move(CaTa1A )    != ConvertibleAllocator(0)   );
        ASSERT(MoveUtil::move(CaTa1A )    == ConvertibleAllocator(&ta1));
        ASSERT(MoveUtil::move(CaTa1A )    != ConvertibleAllocator(&ta2));
        ASSERT(MoveUtil::move(CaDfltB)    == MoveUtil::move(CaDfltA)   );
        ASSERT(MoveUtil::move(CaDfltA)    != MoveUtil::move(CaTa1A )   );
        ASSERT(MoveUtil::move(CaTa1B)     == MoveUtil::move(CaTa1A )   );
        ASSERT(MoveUtil::move(CaTa1B)     != MoveUtil::move(CaDfltB)   );

        // Step 8: Non-'ConvertibleAllocator' arguments
        bslma::Allocator     *const a1Ptr = &ta1;
        bslma::Allocator     *const a2Ptr = &ta2;
        bsl::allocator<char>        ba1(a1Ptr);
        bsl::allocator<int>         ba2(a2Ptr);

        ASSERT(a1Ptr == a1Ptr);
        ASSERT(a1Ptr != a2Ptr);
        ASSERT(a1Ptr == ba1);
        ASSERT(a1Ptr != ba2);
        ASSERT(ba1   == a1Ptr);
        ASSERT(ba2   != a1Ptr);
        ASSERT(ba1   == ba1);
        ASSERT(ba1   != ba2);

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // CONVERSION TO 'bslma::Allocator *'
        //
        // Concerns:
        //: 1 The 'operator Allocator *' method of 'ConvertibleAllocator'
        //:   returns the same result as the 'mechanism' method.
        //: 2 There is an implicit conversion from 'ConvertibleAllocator' to
        //:   'bslma::Allocator *'.  The resulting pointer is the same as that
        //:   returned by the 'mechanism' method.
        //
        // Plan:
        //: 1 Construct multiple 'ConvertibleAllocator' objects and verify that
        //:   the 'operator Allocator *' method, invoked explicitly as a
        //:   member function, returns the address of the allocator used to
        //:   construct the 'ConvertibleAllocator'.  (C-1)
        //: 2 Create a function taking a 'bslma::Allocator *' argument and
        //:   returning its argument.  Verify that this function can be called
        //:   with the 'ConvertibleAllocator' objects from step 1 and that the
        //:   return value matches the address of the allocator used to
        //:   construct the 'ConvertibleAllocator'.  (C-2)
        //
        // Testing:
        //      operator Allocator *() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONVERSION TO 'bslma::Allocator *'"
                            "\n==================================\n");

        using namespace bslma;

        TestAllocator ta;

        ConvertibleAllocator ca1(0);   ConvertibleAllocator& CA1 = ca1;
        ConvertibleAllocator ca2(&ta); ConvertibleAllocator& CA2 = ca2;

        // Step 1: call 'operator Allocator *'
        ASSERT(ca1.operator Allocator *() == Default::defaultAllocator());
        ASSERT(CA1.operator Allocator *() == Default::defaultAllocator());
        ASSERT(ConvertibleAllocator(0).operator Allocator *() ==
               Default::defaultAllocator());

        ASSERT(ca2.operator Allocator *() == &ta);
        ASSERT(CA2.operator Allocator *() == &ta);
        ASSERT(ConvertibleAllocator(&ta).operator Allocator *() == &ta);

        // Step 2: implicit conversion
        ASSERT(convertToBslmaAllocator(ca1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslmaAllocator(CA1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslmaAllocator(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());

        ASSERT(convertToBslmaAllocator(ca2) == &ta);
        ASSERT(convertToBslmaAllocator(CA2) == &ta);
        ASSERT(convertToBslmaAllocator(ConvertibleAllocator(&ta)) == &ta);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // CONVERSION TO 'bsl::allocator'
        //
        // Concerns:
        //: 1 There is an implicit conversion from 'ConvertibleAllocator' to
        //:   'bsl::allocator<char>' that copies the mechanism to the
        //:   resulting object.
        //: 2 For an arbitrary type 'TYPE' that is not 'char', there is an
        //:   implicit conversion from 'ConvertibleAllocator' to
        //:   'bsl::allocator<TYPE>' that copies the mechanism to the resulting
        //:   object.
        //: 3 If passed to a function template taking a 'bsl::allocator<TYPE>'
        //:   argument, 'TYPE' will be deduced to 'char'.
        //
        // Plan:
        //: 1 Create a function template taking a 'bsl::allocator<TYPE>'
        //:   argument by value and returning the result of calling the
        //:   'mechanism' method on its argument.  Verify that this function,
        //:   when instantiated with 'TYPE == char' can be called with a
        //:   'const'-qualified 'ConvertibleAllocator' and that it returns that
        //:   same mechanism.  Perform this test with both a
        //:   null-pointer-constructed 'ConvertibleAllocator' and one
        //:   constructed with the address of a test allocator.  Repeat this
        //:   entire step using a function template that takes a 'const
        //:   ConvertibleAllocator&' argument.  (C-1)
        //: 2 Repeat step 1 except instantiate the test function templates with
        //:   'int' instead of 'char'.  (C-2)
        //: 3. Repeat step 2 except do not provide an explicit type for
        //:   'TYPE'.  (C-3)
        //
        // Testing
        //      CONVERSION TO 'bsl::allocator'
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONVERSION TO 'bsl::allocator'"
                            "\n==============================\n");

        using namespace bslma;

        TestAllocator ta;

        ConvertibleAllocator ca1(0);   ConvertibleAllocator& CA1 = ca1;
        ConvertibleAllocator ca2(&ta); ConvertibleAllocator& CA2 = ca2;


        // Step 1: Test with explicit 'TYPE == char'
        ASSERT(convertToBslmaAllocator(ca1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocator<char>(CA1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocator<char>(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<char>(ca1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<char>(CA1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<char>(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());

        ASSERT(convertToBslAllocator<char>(ca2) == &ta);
        ASSERT(convertToBslAllocator<char>(CA2) == &ta);
        ASSERT(convertToBslAllocator<char>(ConvertibleAllocator(&ta)) == &ta);
        ASSERT(convertToBslAllocatorRef<char>(ca2) == &ta);
        ASSERT(convertToBslAllocatorRef<char>(CA2) == &ta);
        ASSERT(convertToBslAllocatorRef<char>(ConvertibleAllocator(&ta))==&ta);

        // Step 2: Instantiate test with 'TYPE == int'
        ASSERT(convertToBslAllocator<int>(ca1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocator<int>(CA1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocator<int>(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<int>(ca1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<int>(CA1) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef<int>(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());

        ASSERT(convertToBslAllocator<int>(ca2) == &ta);
        ASSERT(convertToBslAllocator<int>(CA2) == &ta);
        ASSERT(convertToBslAllocator<int>(ConvertibleAllocator(&ta)) == &ta);
        ASSERT(convertToBslAllocatorRef<int>(ca2) == &ta);
        ASSERT(convertToBslAllocatorRef<int>(CA2) == &ta);
        ASSERT(convertToBslAllocatorRef<int>(ConvertibleAllocator(&ta))==&ta);

        // Step 3: Deduced 'TYPE == char'
        ASSERT(convertToBslAllocator(ca1) == Default::defaultAllocator());
        ASSERT(convertToBslAllocator(CA1) == Default::defaultAllocator());
        ASSERT(convertToBslAllocator(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef(ca1) == Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef(CA1) == Default::defaultAllocator());
        ASSERT(convertToBslAllocatorRef(ConvertibleAllocator(0)) ==
               Default::defaultAllocator());

        ASSERT(convertToBslAllocator(ca2) == &ta);
        ASSERT(convertToBslAllocator(CA2) == &ta);
        ASSERT(convertToBslAllocator(ConvertibleAllocator(&ta)) == &ta);
        ASSERT(convertToBslAllocatorRef(ca2) == &ta);
        ASSERT(convertToBslAllocatorRef(CA2) == &ta);
        ASSERT(convertToBslAllocatorRef(ConvertibleAllocator(&ta)) == &ta);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // BASIC CONSTRUCTORS
        //
        // Concerns:
        //: 1 If a 'ConvertibleAllocator' is constructed from a null pointer,
        //:   then the mechanism it uses to allocate memory is
        //:   'bslma::Default::defaultAllocator()'.
        //: 2 If a 'ConvertibleAllocator' is constructed from a pointer to
        //:   'bslma::Allocator *' or a class derived from it, then the
        //:   mechanism the 'ConvertibleAllocator' uses to allocate memory is
        //:   the 'bslma::Allocator' object referenced by the pointer.
        //: 3 Construction from 'bslma::Allocator *' is an implicit conversion.
        //: 4 If a 'ConvertibleAllocator' is constructed from a
        //:   'bslma::allocator' object, then mechanism it uses to allocate
        //:   memory is the same as the one used by the specified
        //:   'bsl::allocator' object.
        //: 5 Construction from 'bsl::allocator' is an implicit conversion.
        //: 6 Concerns 4 and 5 apply regardless of the 'value_type' of the
        //:   'bsl::allocator' instantiation.
        //: 7 Concerns 4 and 5 apply regardless of whether the 'bsl::allocator'
        //:   is an lvalue, rvalue (prvalue), or 'MovableRef' (xvalue).
        //
        // Plan:
        //: 1 Temporarily set an object of type 'bslma::TestAllocator' as the
        //:   default allocator.  Construct a 'ConvertibleAllocator' with no
        //:   constructor arguments.  Verify that the 'mechanism' method
        //:   returns the address of that default test allocator.  (C-1)
        //: 2 Construct a 'ConvertibleAllocator' with the address of a
        //:   'bslma::TestAllocator' object.  Verify that the 'mechanism'
        //:   method returns the address of that test allocator.  (C-2)
        //: 3 Create a function taking a 'ConvertibleAllocator' argument by
        //:   value and returning the result of calling the 'mechanism' method
        //:   on its argument.  Verify that this function can be called with
        //:   the address of a test allocator and that it returns that same
        //:   address.  Repeat this step using a function that takes a
        //:   'const ConvertibleAllocator&' argument.  (C-3)
        //: 4 Construct a 'bsl::allocator<char>' from a test allocator and pass
        //:   this allocator to the constructor of 'ConvertibleAllocator'.
        //:   Verify that the 'mechanism' method on the resulting object
        //:   returns the address of the test allocator.  (C-4)
        //: 5 Call same functions as defined step 3, passing them the
        //:   'bsl::allocator<char>' from step 4.  Verify that the returned
        //:   pointer is the address of the original test allocator.  (C-5)
        //: 6 Repeat steps 4 and 5, but with 'bsl::allocator<int>' instead of
        //:   'bsl::allocator<char>'.  (C-6)
        //: 7 Repeat steps 4, 5, and 6 with prvalue and xvalue 'bsl::allocator'
        //:   arguments.  (C-7)
        //
        // Testing:
        //      ConvertibleAllocator(bslma::Allocator *)
        //      ConvertibleAllocator(const bsl::allocator<char>&)
        //      ConvertibleAllocator(bslmf::MovableRef<bsl::allocator<char> >)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC CONSTRUCTORS"
                            "\n==================\n");

        {
            using namespace bslma;

            // Change default allocator for the duration of this test
            TestAllocator         da;
            DefaultAllocatorGuard guard(&da);

            // Step 1: Null-pointer construction
            ConvertibleAllocator  ca1(0);
            ASSERT(&da == ca1.mechanism());
            ASSERT(&da == ConvertibleAllocator(0).mechanism());

            // Step 2: Construction from 'bslma::Allocator *'
            TestAllocator        ta1;
            ConvertibleAllocator ca2(&ta1);
            ASSERT(&ta1 == ca2.mechanism());
            ASSERT(&ta1 == ConvertibleAllocator(&ta1).mechanism());

            Allocator&           a1 = ta1;  // upcast
            ConvertibleAllocator ca3(&a1);
            ASSERT(&ta1 == ca3.mechanism());
            ASSERT(&ta1 == ConvertibleAllocator(&a1).mechanism());

            // Step 3: Implicit conversion from 'bslma::Allocator *'
            ASSERT(&ta1 == convertToConvertibleAllocator(&ta1));
            ASSERT(&ta1 == convertToConvertibleAllocatorRef(&ta1));
            ASSERT(&ta1 == convertToConvertibleAllocator(&a1));
            ASSERT(&ta1 == convertToConvertibleAllocatorRef(&a1));

            // Step 4: Construction from 'bsl::allocator<char>'
            TestAllocator        ta2;
            bsl::allocator<char> a2(&ta2);
            ConvertibleAllocator ca4(a2);
            ASSERT(&ta2 == ca4.mechanism());
            ASSERT(&ta2 == ConvertibleAllocator(a2).mechanism());

            // Step 5: Implicit conversion from  'bsl::allocator<char>'
            ASSERT(&ta2 == convertToConvertibleAllocator(a2));
            ASSERT(&ta2 == convertToConvertibleAllocatorRef(a2));

            // Step 6: Repeat steps 4 and 5 with 'bsl::allocator<int>'
            bsl::allocator<int>  a3(&ta2);
            ConvertibleAllocator ca5(a3);
            ASSERT(&ta2 == ca5.mechanism());
            ASSERT(&ta2 == ConvertibleAllocator(a3).mechanism());

            ASSERT(&ta2 == convertToConvertibleAllocator(a3));
            ASSERT(&ta2 == convertToConvertibleAllocatorRef(a3));

            // Step 7: Repeat steps 4, 5, and 6 with prvalues and xvalues

            // prvalues
            ConvertibleAllocator ca6 = bsl::allocator<char>(&ta2);
            ASSERT(&ta2 == ca6.mechanism());
            ASSERT(&ta2 ==
                 ConvertibleAllocator(bsl::allocator<char>(&ta2)).mechanism());

            ConvertibleAllocator ca7 = bsl::allocator<int>(&ta2);
            ASSERT(&ta2 == ca7.mechanism());
            ASSERT(&ta2 ==
                  ConvertibleAllocator(bsl::allocator<int>(&ta2)).mechanism());

            ASSERT(&ta2 ==
                   convertToConvertibleAllocator(bsl::allocator<char>(&ta2)));
            ASSERT(&ta2 ==
                 convertToConvertibleAllocatorRef(bsl::allocator<char>(&ta2)));

            ASSERT(&ta2 ==
                   convertToConvertibleAllocator(bsl::allocator<int>(&ta2)));
            ASSERT(&ta2 ==
                  convertToConvertibleAllocatorRef(bsl::allocator<int>(&ta2)));

            // xvalues
            ConvertibleAllocator ca8(MoveUtil::move(a2));
            ASSERT(&ta2 == ca8.mechanism());
            ASSERT(&ta2 ==
                   ConvertibleAllocator(MoveUtil::move(a2)).mechanism());

            ConvertibleAllocator ca9(MoveUtil::move(a3));
            ASSERT(&ta2 == ca9.mechanism());
            ASSERT(&ta2 ==
                   ConvertibleAllocator(MoveUtil::move(a3)).mechanism());

            ASSERT(&ta2 == convertToConvertibleAllocator(MoveUtil::move(a2)));
            ASSERT(&ta2 ==
                   convertToConvertibleAllocatorRef(MoveUtil::move(a2)));

            ASSERT(&ta2 == convertToConvertibleAllocator(MoveUtil::move(a3)));
            ASSERT(&ta2 ==
                   convertToConvertibleAllocatorRef(MoveUtil::move(a3)));
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
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace bslma;

        TestAllocator ta;

        // Null-pointer construct
        ConvertibleAllocator defaultCvtAlloc(0);
        ASSERT(Default::defaultAllocator() == defaultCvtAlloc.mechanism());

        // Convert to (legacy) 'bslma::Allocator' pointer
        Allocator *toLegacyAlloc = ConvertibleAllocator(&ta);
        ASSERT(&ta == toLegacyAlloc);

        // Convert to (base class) 'bsl::allocator<char>'
        bsl::allocator<char> charAlloc = ConvertibleAllocator(&ta);
        ASSERT(&ta == charAlloc.mechanism());

        // Convert to (non-base class) 'bsl::allocator<int>'
        bsl::allocator<int> intAlloc = ConvertibleAllocator(&ta);
        ASSERT(&ta == intAlloc.mechanism());

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
// Copyright 2022 Bloomberg Finance L.P.
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
