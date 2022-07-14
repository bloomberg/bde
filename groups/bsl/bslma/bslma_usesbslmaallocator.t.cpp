// bslma_usesbslmaallocator.t.cpp                                     -*-C++-*-

#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component provides a meta-function for associating a trait with a type
// and detecting whether a trait is associated with a type.  Testing involves
// invoking the trait with the different types for which the trait should be
// 'true' as well as other types for which the trait should be 'false',
// including all combinations of cv- and ref-qualifiers.
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] FULL TEST
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

#ifdef BDE_VERIFY
#pragma bde_verify -AL01  // class needs allocator() method
#pragma bde_verify -AP02  // class needs d_allocator_p member
#pragma bde_verify -AT02  // class uses allocator but does not have trait
#pragma bde_verify -FD01: // Function declaration requires contract
#pragma bde_verify -FD03  // parameter not documented
#pragma bde_verify -TP19
#endif

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

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMA_STDALLOCATOR
// We don't want to create a dependency on 'bslma_stdallocator', so, for
// testing purposes, we copy the saliant parts of its interface here.

namespace bsl {

template <class TYPE>
class allocator {

  public:
    typedef TYPE value_type;

    allocator();
    allocator(bslma::Allocator *);                                  // IMPLICIT

    TYPE *allocate(std::size_t);
    void deallocate(TYPE *, std::size_t);
};

}  // close namespace bsl

#endif // ! INCLUDED_BSLMA_STDALLOCATOR

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Associating the 'bslma' Allocator Trait with a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to declare two types that make use of a 'bslma' allocator,
// and need to associate the 'UsesBslmaAllocator' trait with those types (so
// that they behave correctly when inserted into a 'bsl' container, for
// example).  In this example we will demonstrate two different mechanisms by
// which a trait may be associated with a type.
//
// First, we define a type 'UsesAllocatorType1' and use the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with the
// 'UsesBslmaAllocator' trait:
//..
    namespace xyz {

    class UsesAllocatorType1 {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
                                       bslma::UsesBslmaAllocator);
        // CREATORS
        explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
            // ...

        UsesAllocatorType1(const UsesAllocatorType1&  original,
                           bslma::Allocator          *basicAllocator = 0);
            // ...
    };
//..
// Note that the macro declaration must appear within the scope of the class
// declaration and must have 'public' access.
//
// Next, we define a type 'UsesAllocatorType2' and define a specialization of
// the 'UsesBslmaAllocator' trait for 'UsesAllocatorType2' that associates the
// 'UsesBslmaAllocator' trait with the type (sometimes referred to as a
// "C++11-style" trait declaration, because it is more in keeping with the
// style of trait declarations found in the C++11 Standard).  Note that the
// specialization must be performed in the 'BloombergLP::bslma' namespace:
//..
    class UsesAllocatorType2 {
        // ...

      public:
        // CREATORS
        explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
            // ...

        UsesAllocatorType2(const UsesAllocatorType2&  original,
                           bslma::Allocator          *basicAllocator = 0);
            // ...
    };

    }  // close namespace xyz

    // TRAITS
    namespace BloombergLP {
    namespace bslma {

    template <>
    struct UsesBslmaAllocator<xyz::UsesAllocatorType2> : bsl::true_type
    {};

    }  // close package namespace
    }  // close enterprise namespace
//..
// Next, we define a type 'BslAAClass', which provides a bsl-AA interface
// based on 'bsl::allocator':
//..
    namespace xyz {

    class BslAAClass {
        // ...

      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;

        // CREATORS
        explicit BslAAClass(const allocator_type& alloc = allocator_type());

        BslAAClass(const BslAAClass&     other,
                   const allocator_type& alloc = allocator_type());
    };
//..
// Finally, we define a type 'NonAAClass', which is derived from 'BslAAClass'
// but is not AA itself.  Because it inherits 'allocator_type', this class
// explicitly says it is *not* AA by overiding 'allocator_type' with 'void':
//..
    class NonAAClass : public BslAAClass {
        // ...

      public:
        // TYPES
        typedef void allocator_type;  // Not an AA class

        // CREATORS
        NonAAClass();

        NonAAClass(const NonAAClass& other);
    };
    }  // close namespace xyz
//..
//
///Example 2: Testing Whether a Type Uses a 'bslma' Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to test whether each of a set of different types uses a
// 'bslma' allocator.
//
// First, we define a class that is not AA, for control purposes:
//..
    namespace xyz {

    class DoesNotUseAnAllocatorType { };

    }  // close namespace xyz
//..
// Now, we use the 'UsesBslmaAllocator' template to test whether this type, and
// any of the other types in Example 1 use bslma allocators:
//..
    void usageExample2()
    {
        using namespace xyz;

        ASSERT(! bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
        ASSERT(bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
        ASSERT(bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
        ASSERT(bslma::UsesBslmaAllocator<BslAAClass>::value);
        ASSERT(!bslma::UsesBslmaAllocator<NonAAClass>::value);
    }
//..
// Finally, we demonstrate that the trait can be tested at compilation time by
// testing the trait within the context of a compile-time 'BSLMF_ASSERT':
//..
    BSLMF_ASSERT(
            !bslma::UsesBslmaAllocator<xyz::DoesNotUseAnAllocatorType>::value);
    BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::UsesAllocatorType1>::value);
    BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::UsesAllocatorType2>::value);
    BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::BslAAClass>::value);
    BSLMF_ASSERT(!bslma::UsesBslmaAllocator<xyz::NonAAClass>::value);
//..


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct DerivedAllocator : bslma::Allocator {
    // A 'bslma'-style allocator type.
    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;  // Never called
    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;  // Never called
};

void *DerivedAllocator::allocate(size_type) { return 0; }
void DerivedAllocator::deallocate(void *) { }

DerivedAllocator unusedObj;  // Silence warnings on unused virtual functions

struct BslmaCompatibleSTLAllocator {
    // This STL-like allocator class is implicitly convertible from
    // 'bslma::Allocator*'.  A class where 'allocator_type' is an alias for
    // this class should implicitly have the 'UsesBslmaAllocator' trait.

    typedef int value_type;

    BslmaCompatibleSTLAllocator(bslma::Allocator *);                // IMPLICIT

    value_type* allocate(std::size_t);          // No body needed for this test
    void deallocate(value_type *, std::size_t); // No body needed for this test
};

struct HasSniffableTrait {
    // The 'UsesBslmaAllocator' trait is sniffable (i.e., detectable as being
    // 'true') from this class's conversion constructor.  Note that this method
    // of associating the trait with a class is deprecated.

    HasSniffableTrait(bslma::Allocator *);                          // IMPLICIT
        // Create a 'HasSniffableTrait' object.

    HasSniffableTrait(const HasSniffableTrait&, bslma::Allocator *);
        // Extended copy constructor
};

struct HasExplicitlyTrueTrait {
    // This class has the 'UsesBslmaAllocator' trait explicitly specialized as
    // 'true_type', below.
};

struct HasExplicitlyFalseTrait {
    // This class has the 'UsesBslmaAllocator' trait explicitly specialized as
    // 'false_type', below.  Note that the explicit specialization overrides
    // the implicitly sniffed trait arizing from the implicit constructor.

    HasExplicitlyFalseTrait(bslma::Allocator *);                    // IMPLICIT
};

struct HasNestedTrait {
    // This class defines the 'UsesBslmaAllocator' trait to be 'true' by means
    // of 'BSLMF_NESTED_TRAIT_DECLARATION'.
    BSLMF_NESTED_TRAIT_DECLARATION(HasNestedTrait, bslma::UsesBslmaAllocator);
};

class NotCopyConstructible {
    // Class with private copy constructor.
    NotCopyConstructible(const NotCopyConstructible&);

  public:
    NotCopyConstructible();
};

class HasSniffableAndNestedTrait {
    // The 'UsesBslmaAllocator' trait is sniffable (i.e., detectable as being
    // 'true') from this class's conversion constructor.  Additionally, this
    // class defines the 'UsesBslmaAllocator' trait to be 'true' by means of
    // 'BSLMF_NESTED_TRAIT_DECLARATION'.

    NotCopyConstructible d_obj;
        // Prevent generation of implicit copy constructor.  Trait sniffing
        // has been known to cause compilation errors on IBM by instantiating
        // a deleted implicit copy constructor.  However, the explicit trait
        // *should* suppress that error because sniffing is not needed in that
        // case.

  public:

    HasSniffableAndNestedTrait(bslma::Allocator *);                 // IMPLICIT
        // Create a 'HasSniffableAndNestedTrait' object.

    BSLMF_NESTED_TRAIT_DECLARATION(HasSniffableAndNestedTrait,
                                   bslma::UsesBslmaAllocator);
};

struct HasCompatibleAllocatorType {
    // This class defines a nested 'allocator_type' that is an alias for a type
    // convertible from 'bslma::Allocator *'.  'UsesAbslmaAllocator' should be
    // 'true' for such cases, but was unintuitively 'false' prior to
    // 'BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE' being turned on.

    typedef BslmaCompatibleSTLAllocator allocator_type;
};

struct EmptyClass {
};

struct ConstructibleFromAnyPointer {
    // Class implicitly convertible from any pointer type.  Even though it is
    // implicitly convertible from 'bslma::Allocator *', the
    // 'UsesBslmaAllocator' trait should be 'false' for this type.

    template <class TYPE>
    ConstructibleFromAnyPointer(TYPE *);                            // IMPLICIT
};

struct HasIncompatibleAllocatorType {
    // The 'BslmaUsesAllocator' should be 'false' for this type even though it
    // has an 'allocator_type' member because there is no implicit conversion
    // from 'bslma::Allocator *' to 'allocator_type'.

    struct allocator_type { };
};

struct ConvertibleToAny {
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.

    template <class TYPE>
    operator TYPE() const { return TYPE(); }
        // Return a default constructed object of 'TYPE'.
};

}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<HasExplicitlyTrueTrait> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<HasExplicitlyFalseTrait> : bsl::false_type {};

template <>
struct UsesBslmaAllocator<ConvertibleToAny> : bsl::true_type {
    // Even though the nested trait logic is disabled by the template
    // conversion operator, the out-of-class trait specialization should still
    // work.
};

}  // close package namespace
}  // close enterprise namespace

namespace {

template <bool TF>
BSLS_KEYWORD_CONSTEXPR bool isTrueType(const bsl::integral_constant<bool, TF>&)
    // Return 'true' if the argument is convertible to 'bsl::true_type' and
    // 'false' if the argument is convertible to 'bsl::false_type'.  A call to
    // this function template will fail during overload resolution unless the
    // argument is convertible to one of 'true_type' or 'false_type'.
{
    return TF;
}

template <class TYPE, bool EXP>
void testTrait(const int L)
    // For a call from the specified line, 'L', verify that
    // 'bslma::UsesBslmaAllocator<TYPE>::value' equals 'EXP' and that adding
    // cv-qualifiers to 'TYPE' does not change the result, but that adding a
    // reference qualifier to 'TYPE' always produces a 'false' result.
{
    using bslma::UsesBslmaAllocator;

    // Check with all CV qualifiers
    ASSERTV(L, EXP == isTrueType(UsesBslmaAllocator<TYPE>()));
    ASSERTV(L, EXP == isTrueType(UsesBslmaAllocator<const TYPE>()));
    ASSERTV(L, EXP == isTrueType(UsesBslmaAllocator<volatile TYPE>()));
    ASSERTV(L, EXP == isTrueType(UsesBslmaAllocator<const volatile TYPE>()));

    // Always false with lvalue reference qualifiers
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<TYPE&>()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<const TYPE&>()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<volatile TYPE&>()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<const volatile TYPE&>()));

    using bslmf::MovableRef;
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<MovableRef<TYPE> >()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<MovableRef<const TYPE> >()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<MovableRef<volatile TYPE> >()));
    ASSERTV(L, !isTrueType(UsesBslmaAllocator<
                           MovableRef<const volatile TYPE> >()));
}

}  // close unnamed namespace

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

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 If 'TYPE' is implicitly convertible from 'bslma::Allocator *',
        //:   then 'UsesBslmaAllocator<TYPE>' derives from 'bsl::true_type'.
        //: 2 If 'bslma::UsesBslmaAllocator<TYPE>' is explicitly specialized
        //:   for 'TYPE', then the specialization will be used instead of the
        //:   primary template.
        //: 3 If 'TYPE' is a class that explicitly associates itself with the
        //:   'bslma::UsesBslmaAllocator' trait by means of the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro, then
        //:   'UsesBslmaAllocator<TYPE>' derives from 'bsl::true_type'.
        //: 4 If 'TYPE' has a nested type, 'allocator_type', and if
        //:   'bslma::Allocator *' is convertible to 'allocator_type', then
        //:   'UsesBslmaAllocator<TYPE>' derives from 'bsl::true_type'.
        //: 5 Otherwise, if none of the above conditions are true, then
        //:   'UsesBslmaAllocator<TYPE>' derives from 'bsl::false_type'.  This
        //:   concern applies to all pointer types (even thouse convertible
        //:   to 'bslma::Allocator'), all reference types, and all function
        //:   types.
        //: 6 Concern 1 does not apply if 'TYPE' is convertible from *any*
        //:   arbitrary pointer, regardless of whether that pointer is related
        //:   to 'bslma::Allocator*'.  In this case, 'UsesBslmaAllocator<TYPE>'
        //:   derives from 'bsl::false_type' unless one of the conditions in
        //:   concerns 2 through 4 apply.
        //: 7 Condition 4 does not apply to 'TYPE's having an 'allocator_type'
        //:   that is *not* convertible from 'bslma::Allocator *'.  In this
        //:   case, 'UsesBslmaAllocator<TYPE>' derives from 'bsl::false_type'
        //:   unless one of the conditions in concerns 1 through 3 apply.
        //: 8 The cv-qualification of a 'TYPE' has no effect on the result of
        //:   invoking 'UsesBslmaAllocator<TYPE>'.
        //
        // Plan:
        //: 1 Define a class that is implicitly constructible from
        //:   'bslma::Allocator*', verify that 'UsesBslmaAllocator' is 'true'
        //:   for that type.  (C-1)
        //: 2 Define a pair of classes, and explicitly specialize
        //:   'UsesBslmaAllocator' for each -- one derived from 'true_type',
        //:   the other derived from 'false_type'.  Verify that instantiating
        //:   the trait for each type yields the value of the specialization.
        //:   (C-2)
        //: 3 Define a class that uses 'BSLMF_NESTED_TRAIT_DECLARATION' to
        //:   associate itself with the 'UsesBslmaAllocator' trait.  Verify
        //:   that invoking the trait on that class type yields 'true'.  (C-3)
        //: 4 Define an STL-style allocator that is implicitly constructible
        //:   from 'bslma::Allocator *' and define a class with a nested
        //:   'allocator_type' alias to that STL-style allocator.  Verify that
        //:   invoking 'UsesBslmaAllocator' on that class yields 'true'.  (C-4)
        //: 5 Invoke 'UsesBslmaAllocator' on scalar types, pointer types,
        //:   reference types, function types, and empty class types and verify
        //:   that the result is 'false'.  (C-5)
        //: 6 Define a class having an implicit constructor template that
        //:   accepts a pointer to any pointer type but is otherwise not an
        //:   allocator-aware type.  Verify that 'UsesBslmaAllocator' yields
        //:   false for that class.  (C-6)
        //: 7 Define a class having a nested 'allocator_type' that is not
        //:   convertible from 'bslma::Allocator*'. Verify that
        //:   'UsesBslmaAllocator' yields false for that class.  (C-7)
        //: 8 Repeat steps 1 through 7, adding cv qualifications to each type
        //:   and verify that the results are the same.  (C-8)
        //
        // Testing:
        //     FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

#define TEST(TYPE, EXP) testTrait<TYPE, EXP>(L_)

        //   Type                           Exp
        //   ----------------------------   -----
        TEST(HasSniffableTrait            , true );  // Step 1

        TEST(HasExplicitlyTrueTrait       , true );  // Step 2
        TEST(HasExplicitlyFalseTrait      , false);  // Step 2
        TEST(ConvertibleToAny             , true );  // Step 2

        TEST(HasNestedTrait               , true );  // Step 3
        TEST(HasSniffableAndNestedTrait   , true );  // Step 3

#ifdef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
        TEST(HasCompatibleAllocatorType   , true );  // Step 4
#else
        TEST(HasCompatibleAllocatorType   , false);  // Step 4
#endif

        TEST(int                          , false);  // Step 5
        TEST(EmptyClass                   , false);  // Step 5
        TEST(void *                       , false);  // Step 5
        TEST(const void *                 , false);  // Step 5
        TEST(bslma::Allocator *           , false);  // Step 5
        TEST(const bslma::Allocator *     , false);  // Step 5
        TEST(DerivedAllocator *           , false);  // Step 5

        TEST(ConstructibleFromAnyPointer  , false);  // Step 6

        TEST(HasIncompatibleAllocatorType , false);  // Step 7

        // Note that step 8 is built into the 'testTrait' function template.

#undef TEST

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

        using bslma::UsesBslmaAllocator;

        ASSERT(UsesBslmaAllocator<HasSniffableTrait>::value);
        ASSERT(UsesBslmaAllocator<HasNestedTrait>::value);

        ASSERT(! UsesBslmaAllocator<int>::value);
        ASSERT(! UsesBslmaAllocator<void *>::value);
        ASSERT(! UsesBslmaAllocator<bslma::Allocator *>::value);

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
