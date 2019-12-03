// bslmf_detectnestedtrait.t.cpp                                      -*-C++-*-

#include <bslmf_detectnestedtrait.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_assert.h>                   // usage example
#include <bslmf_integralconstant.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// [ 2] DetectNestedTrait::type
// [ 2] DetectNestedTrait::value
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 3] QoI: diagnose incomplete types
// [ 4] CONCERN: DETECTING TRAITS ON ARRAYS

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

//=============================================================================
//              PLATFORM-SPECIFIC MACROS FOR WORKAROUNDS
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
# define BSLMF_DETECTNESTEDTRAIT_NO_ABOMINABLE_TYPES    1
#endif

// ============================================================================
//                      TEST DRIVER CONFIGURATION MACROS
// ----------------------------------------------------------------------------

// Enable the macros below to configure specific builds of the test driver for
// manual testing

//#define BSLMF_DETECTNESTEDTRAIT_SHOW_ERROR_FOR_INCOMPLETE_CLASS  1
    // Define this macro to test for compile-fail diagnostics instantiating
    // 'DetectNestedTrait' with an incomplete class type.

//#define BSLMF_DETECTNESTEDTRAIT_SHOW_ERROR_FOR_INCOMPLETE_UNION  1
    // Define this macro to test for compile-fail diagnostics instantiating
    // 'DetectNestedTrait' with an incomplete union type.

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: push    // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -FD01   // Function contracts replaced by expository text

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a Custom Trait
/// - - - - - - - - - - - - - - - - -
// When writing generic infrastructure code, we often need to choose among
// multiple code paths based on the capabilities of the types on which we are
// operating.  If those capabilities are reflected in a type's public
// interface, we may be able to use techniques such as SFINAE to choose the
// appropriate code path.  However, SFINAE cannot detect all of a type's
// capabilities.  In particular, SFINAE cannot detect constructors, memory
// allocation, thread-safety characteristics, and so on.  Functions that depend
// on these capabilities must use another technique to determine the correct
// code path to use for a given type.  We can solve this sort of problem by
// associating types with custom traits that indicate what capabilities are
// provided by a given type.
//
// First, in package 'abcd', define a trait, 'RequiresLockTrait', that
// indicates that a type's methods must not be called unless a known lock it
// first acquired:
//..
    namespace abcd {

    template <class TYPE>
    struct RequiresLockTrait :
                      bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type {
    };

    }  // close namespace abcd
//..
// Notice that 'RequiresLockTrait' derives from
// 'bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type' using the
// curiously recurring template pattern.
//
// Then, in package 'xyza', we declare a type, 'DoesNotRequireALockType', that
// can be used without acquiring the lock:
//..
    namespace xyza {

    class DoesNotRequireLockType {
        // ...

      public:
        // CREATORS
        DoesNotRequireLockType();
            // ...
    };
//..
// Next, we declare a type, 'RequiresLockTypeA', that does require the lock.
// We use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with
// the 'abcd::RequiresLockTrait' trait:
//..
    class RequiresLockTypeA {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(RequiresLockTypeA,
                                       abcd::RequiresLockTrait);

        // CREATORS
        RequiresLockTypeA();
            // ...

    };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a templatized container type, 'Container', that is
// parameterized on some 'ELEMENT' type.  If 'ELEMENT' requires a lock, then a
// 'Container' of 'ELEMENT's will require a lock as well.  This can be
// expressed using the 'BSLMF_NESTED_TRAIT_DECLARATION_IF' macro, by providing
// 'abcd::RequiresLockTrait<ELEMENT>::value' as the condition for associating
// the trait with 'Container'.
//..
    template <class ELEMENT>
    struct Container {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, abcd::RequiresLockTrait,
                                      abcd::RequiresLockTrait<ELEMENT>::value);

        // ...
    };
//..
// Next, we show that traits based on 'bslmf::DetectNestedTrait' can be
// associated with a type using "C++11-style" trait association.  To do this,
// we declare a type, 'RequiresLockTypeB', that also requires the lock, but
// does not used the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
//..
    class RequiresLockTypeB {
        // ...

      public:
        // CREATORS
        RequiresLockTypeB();
            // ...

    };

    }  // close namespace xyza
//..
// Then, we associate 'RequiresLockTypeB' with 'abcd::RequiresLockTrait' by
// directly specializing 'abcd::RequiresLockTrait<xyza::RequiresLockTypeB>'.
// This is the standard way of associating a type with a trait since C++11:
//..
    namespace abcd {

    template <>
    struct RequiresLockTrait<xyza::RequiresLockTypeB> : bsl::true_type {
    };

    }  // close namespace abcd
//..
// Now, we can write a function that inspects
// 'abcd::RequiresLockTrait<TYPE>::value' to test whether or not various types
// are associated with 'abcd::RequiresLockTrait':
//..
    void example1()
    {
        ASSERT(false ==
               abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);

        ASSERT(false ==
               abcd::RequiresLockTrait<
                       xyza::Container<xyza::DoesNotRequireLockType> >::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeA> >::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeB> >::value);

        // ...
    }
//..
// Finally, we demonstrate that the trait can be tested at compilation time, by
// writing a function that tests the trait within the context of a compile-time
// 'BSLMF_ASSERT':
//..
    void example2()
    {
        BSLMF_ASSERT(false ==
               abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);

        BSLMF_ASSERT(false ==
               abcd::RequiresLockTrait<
                       xyza::Container<xyza::DoesNotRequireLockType> >::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeA> >::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeB> >::value);
    }
//..

// BDE_VERIFY pragma: pop  // end of usage example-example relaxed rules

// ============================================================================
//                      GLOBAL MACROS FOR TESTING
// ----------------------------------------------------------------------------

// Macro: ASSERT_DETECT_NESTED_TRAIT
//  This macro tests that 'DetectNestedTrait' has the expected 'RESULT' when
//  detecting the 'foo::IsExpandable' trait for the given 'TYPE'.
#define ASSERT_DETECT_NESTED_TRAIT(TYPE, RESULT)                              \
    {                                                                         \
        typedef bslmf::DetectNestedTrait<TYPE, foo::IsInflatable> TestTrait;  \
                                                                              \
        ASSERT( RESULT == TestTrait::value );                                 \
        ASSERT( RESULT == dispatchForConversion(TestTrait()));                \
        ASSERT( RESULT == dispatchOnType((TestTrait*)0));                     \
        ASSERT( RESULT == dispatchOnType((TestTrait::type *)0));              \
    }
    // Test that the result of 'bslmf::DetectNestedTrait::value' for a supplied
    // 'TYPE', querying support for the 'foo::IsExpandable' trait, has the same
    // 'value' as the expected 'RESULT'.  Then confirm that the resulting trait
    // is implicitly convertible to 'bsl::true_type' or 'bsl::false_type'
    // accordingly, and that the nested typedef 'type' is an alias for either
    // 'bsl::true_type' or 'bsl::false_type' accordingly.


// Macro: ASSERT_DETECT_NESTED_TRAIT_FOR_RVAL_REF
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define ASSERT_DETECT_NESTED_TRAIT_FOR_RVAL_REF(TYPE, RESULT)                \
    ASSERT_DETECT_NESTED_TRAIT(bsl::add_rvalue_reference<TYPE>::type, false)
#else
# define ASSERT_DETECT_NESTED_TRAIT_FOR_RVAL_REF(TYPE, RESULT)
#endif


// Macro: ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE
//  This macro tests that 'DetectNestedTrait' has the expected 'RESULT' when
//  detecting the 'foo::IsExpandable' trait for the given 'TYPE', and that
//  pointers/references to that type never have a nested trait.
# define ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(TYPE, RESULT)                    \
    ASSERT_DETECT_NESTED_TRAIT(TYPE, RESULT);                                 \
    ASSERT_DETECT_NESTED_TRAIT(bsl::add_pointer<TYPE>::type, false);          \
    ASSERT_DETECT_NESTED_TRAIT(bsl::add_lvalue_reference<TYPE>::type, false); \
    ASSERT_DETECT_NESTED_TRAIT_FOR_RVAL_REF(TYPE, RESULT)


// Macro: ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE
//  This macro tests that 'DetectNestedTrait' has the expected 'RESULT' when
//  detecting the 'foo::IsExpandable' trait for the given 'TYPE', and all
//  cv-qualified variations of that type, and pointers and references to each
//  of those cv-qualified types.
#define ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE, RESULT)                  \
    ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(TYPE, RESULT);                        \
    ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(bsl::add_cv<TYPE>::type, RESULT)      \
    ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(bsl::add_const<TYPE>::type, RESULT);  \
    ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(bsl::add_volatile<TYPE>::type, RESULT);


// Macro: ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE
//  This macro tests that 'DetectNestedTrait' has the expected 'RESULT' when
//  detecting the 'foo::IsExpandable' trait for the given 'TYPE', and all
//  cv-qualified variations of that type, and pointers and references to each
//  of those cv-qualified types, and arrays of those cv-qualified types.  Note
//  that this macro does not recursively test arrays of pointers to 'TYPE'.
#define ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(TYPE, RESULT);             \
    ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE, RESULT);                     \
    ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE[128], false);                 \
    ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE[12][8], false);               \
    ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE[], false);                    \
    ASSERT_DETECT_NESTED_TRAIT_FOR_CV_TYPE(TYPE[][8], false)

// ============================================================================
//                      SUPPORT FUNCTIONS FOR BREATHING TEST
// ----------------------------------------------------------------------------

// The following two overloaded functions are used by the global macros above
// to validate base characteristics of a type trait: namely, that it is
// publicly and unambiguously derived from to its base characteristic, and that
// the public members of that base characteristic, such as the nested typename
// 'type', remain unambiguous and publicly accessible (including conversion to
// said base characteristic through its copy constructor).

bool dispatchForConversion(bsl::false_type) { return false; }
bool dispatchForConversion(bsl::true_type)  { return true;  }
    // Return 'true' if the supplied argument is convertible to
    // 'bsl::true_type', and 'false' if the supplied argument is convertible to
    // 'bsl::false_type'; otherwise calls to this function will fail to
    // compile.


bool dispatchOnType(bsl::false_type *) { return false; }
bool dispatchOnType(bsl::true_type *)  { return true;  }
void dispatchOnType(void *);
    // Return 'true' if the supplied argument is a pointer to 'bsl::true_type',
    // and 'false' if the supplied argument is a pointer to 'bsl::false_type';
    // otherwise calls to this function with any other pointer type, such as a
    // pointer to a class type publicly derived from 'bsl::true_type' or
    // 'bsl::false_type', will match the third overload that is never defined
    // and does not have a return value, and so should produce some compilation
    // failure.

// ============================================================================
//                      TYPES TO SUPPORT TESTING
// ----------------------------------------------------------------------------

// In order to demonstrate that there are no ambiguities buried in the traits
// customization facility, define two traits having the same unqualified name,
// but in different namespaces when the fully qualified name is used.  This
// will also allow us to demonstrate that looking for a trait with the same
// name but from another namespace will not be found unless a class is
// associated with both traits.

namespace foo {
// Note curiously-recurring template pattern
template <class TYPE>
struct IsInflatable : bslmf::DetectNestedTrait<TYPE, IsInflatable>::type {
    // This trait derives from 'bsl::true_type' if (template parameter) 'TYPE'
    // is defined using the nested trait declaration for this 'IsInflatable'
    // trait, and from 'bsl::false_type' otherwise.
};
}  // close namespace foo

namespace bar {
// Note curiously-recurring template pattern
template <class TYPE>
struct IsInflatable : bslmf::DetectNestedTrait<TYPE, IsInflatable>::type {
    // This trait derives from 'bsl::true_type' if (template parameter) 'TYPE'
    // is defined using the nested trait declaration for this 'IsInflatable'
    // trait, and from 'bsl::false_type' otherwise.
};
}  // close namespace bar


// Next we define several classes and unions associated with various traits.
// In order to demonstrate that the trait association metaprogram depends only
// on type names, all of these types will have a private copy constructor and
// not default constructor: it is essentially impossible to create values of
// any of these types are runtime.

class InflatableClass {
    // This 'struct' represents an inflatable type.
  private:
    // NOT IMPLEMENTED
    InflatableClass(const InflatableClass&);

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(InflatableClass, foo::IsInflatable);
};

struct InflatableDerived : InflatableClass {
    // This 'struct' demonstrates that nested traits are not inherited.
};

union InflatableUnion {
    // This union represents an inflatable type.

    BSLMF_NESTED_TRAIT_DECLARATION(InflatableUnion, foo::IsInflatable);
};

struct NonflatableClass {
    // This 'struct' represents a non-inflatable type (for 'foo') that has an
    // associated trait with the same name, but from another namespace.
  private:
    // NOT IMPLEMENTED
    NonflatableClass(const NonflatableClass&);

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(NonflatableClass, bar::IsInflatable);
};

union NonflatableUnion {
    // This 'union' represents a non-inflatable type (for 'foo') that has an
    // associated trait with the same name, but from another namespace.

    BSLMF_NESTED_TRAIT_DECLARATION(NonflatableUnion, bar::IsInflatable);
};

// Then, we define a class and a union type associated with two traits having
// the same (unqualified) name in each of their respective namespaces.

struct ExpandedClass {
    // This 'struct' represents an inflatable type in two dimensions of
    // inflatability, in order to support testing for ambiguity concerns.
  private:
    // NOT IMPLEMENTED
    ExpandedClass(const ExpandedClass&);

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(ExpandedClass, foo::IsInflatable);
    BSLMF_NESTED_TRAIT_DECLARATION(ExpandedClass, bar::IsInflatable);
};

union ExpandedUnion {
    // This 'union' represents an inflatable type in two dimensions of
    // inflatability, in order to support testing for ambiguity concerns.

    BSLMF_NESTED_TRAIT_DECLARATION(ExpandedUnion, foo::IsInflatable);
    BSLMF_NESTED_TRAIT_DECLARATION(ExpandedUnion, bar::IsInflatable);
};

// Next, we define empty and incomplete types that are not associated with any
// traits.  Note that the incomplete types are used mostly to support
// compile-fail tests.

struct EmptyClass {};
struct EmptyUnion {};

class Incomplete;
union Uncomplete;

// Then, we define a class template that is conditionally associated with a
// trait only if the template argument used to instantiate it is similarly
// associated with that trait.

template <class TYPE>
struct Container {
    // 'Container' is an inflatable type if, and only if, 'TYPE' is inflatable.

    BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, foo::IsInflatable,
                                      foo::IsInflatable<TYPE>::value);
};

// Next, for completeness, we create an enum and enum-class type to demonstrate
// that enumerations do not support nested traits.  We do not test the simple
// case of associating a trait with these enumerations via explicit template
// instantiation.

enum NonflatableEnum {
    // This 'enum' demonstrates that enumerations cannot be associated with a
    // nested trait.

    e_VALUE = 0
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum class NonflatableEnumClass {
    // This 'enum class' demonstrates that enumerations cannot be associated
    // with a nested trait.

    e_VALUE = 0
};
#endif

// Finally, we define a class specifically intended to fool the trait detection
// mechanism by being convertible to anything.

struct ConvertibleToAny {
    // Type that can be converted to any type.  'DetectNestedTrait' should not
    // assign it any traits.  The concern to test is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator should not interfere with the nested
    // trait logic.  Note that this means it is impossible to associate /any/
    // traits with this 'ConvertibleToAny' type.  However, traits can still be
    // explicitly specialized for this and similar types.

    template <class TYPE>
    operator TYPE() const { return TYPE(); }
        // Return a default-constructed object of the deduced 'TYPE'.
};


// ============================================================================
//                      DETECTING TRAITS ON ARRAYS
// ----------------------------------------------------------------------------

namespace DetectTraitsOnArrays {

// Note that the trait declaration that follows is patterend in
// 'bslalg_typetraithasstliterators'.  This is based on the failure observed
// in internal ticket: 84805986.

template <class TYPE>
struct HasMyTrait : bslmf::DetectNestedTrait<TYPE, HasMyTrait> {
};

struct TypeTraitHasMyTrait {
    // A type with this trait defines (at minimum) the nested types 'iterator'
    // and 'const_iterator' and the functions 'begin()' and 'end()' having the
    // standard STL semantics.

    template <class TYPE>
    struct NestedTraitDeclaration
        : bslmf::NestedTraitDeclaration<TYPE, HasMyTrait> {
        // This class template ties the 'bslalg::TypeTraitHasMyTrait' trait tag
        // to the 'bslmf::HasMyTrait' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : HasMyTrait<TYPE>::type { };
};


template <class TYPE>
inline
int doSomething(const TYPE&)
{
    return HasMyTrait<TYPE>::value;
}

template <class TYPE>
inline
int doSomethingWrapper(const TYPE& target)
{
    return doSomething(target);
}

int testFunction()
{
    char b[10][5];
    return doSomethingWrapper(b);
}

}  // close namespace DetectTraitsOnArrays

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

    (void)        veryVerbose;  // suppress warning
    (void)    veryVeryVerbose;  // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        example1();
        example2();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: DETECTING TRAITS ON ARRAYS
        //   The XLC compiler sometimes treats array template parameters in
        //   non-standard ways, which has resulted in 'DetectNestedTraits'
        //   failing to compile (see internal ticket: 84805986).
        //
        // Concerns:
        //: 1 That a work-around for the XLC compiler successfully allows
        //:   compiling code that attempts to detect a trait on an array.
        //
        // Plan:
        //: 1 Implement a bogus trait (based on
        //:   'bslalg_typetraithasstliterator').  Attempt to detect this trait
        //:   on a 'const char[][]' type. (C-1)
        //
        // Testing:
        //   CONCERN: DETECTING TRAITS ON ARRAYS
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCERN: DETECTING TRAITS ON ARRAYS"
                            "\n===================================\n");

        if (verbose) {
            // Note that this test is successful if 'testFunction' compiles.
            printf("%d", DetectTraitsOnArrays::testFunction());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES
        //  It is not possible to determine whether an incomplete class type
        //  has a nested trait as we may produce different results once the
        //  type is complete, leading to difficult to diagnose ODR violations.
        //
        // Concerns:
        //:  1 Instantiating 'bslmf::DetectNestedTrait' with an incomplete
        //:    class (or union) type is clearly diagnosed with an error that
        //:    cannot be circumvented by template tricks exploiting SFINAE.
        //:
        //:  2 Pointers and references to incomplete types are perfectly valid
        //:    types for which the trait always returns 'false'.
        //
        // Plan:
        //:  1 Under a pre-processor check requiring manual enablement, try to
        //:    instantiate 'bslmf::DetectNestedTrait' for incomplete class
        //:    types.
        //:
        //:  2 Verify that pointers and references to incomplete class types
        //:    return 'false' for this trait.
        //
        // Testing:
        //   QoI: diagnose incomplete types
        // --------------------------------------------------------------------

        if (verbose)
               printf("\nTESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES"
                      "\n=================================================\n");

        // C-1
#if defined(BSLMF_DETECTNESTEDTRAIT_SHOW_ERROR_FOR_INCOMPLETE_CLASS)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(Incomplete,   false);
#endif

#if defined(BSLMF_DETECTNESTEDTRAIT_SHOW_ERROR_FOR_INCOMPLETE_UNION)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(Uncomplete,   false);
#endif

        // C-2
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(Incomplete *, false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(Uncomplete *, false);

        ASSERT_DETECT_NESTED_TRAIT(Incomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(const Incomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(volatile Incomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(const volatile Incomplete&, false);

        ASSERT_DETECT_NESTED_TRAIT(Uncomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(const Uncomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(volatile Uncomplete&, false);
        ASSERT_DETECT_NESTED_TRAIT(const volatile Uncomplete&, false);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_DETECT_NESTED_TRAIT(Incomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(const Incomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(volatile Incomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(const volatile Incomplete&&,false);

        ASSERT_DETECT_NESTED_TRAIT(Uncomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(const Uncomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(volatile Uncomplete&&, false);
        ASSERT_DETECT_NESTED_TRAIT(const volatile Uncomplete&&,false);
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAIT IS CORRECT METAFUNCTION
        //   The class template 'bslmf::DetectNestedTrait' models a standard
        //   predicate trait, which derives from 'bsl::true_type' if the
        //   queried type is associated with the specified nested trait, and
        //   from 'bsl::false_type' otherwise, for any valid type.
        //
        // Concerns:
        //: 1 'DetectNestedTrait<TYPE, TRAIT>' compiles and gives the expected
        //:   result for every category of type.
        //:
        //: 2 Class types (including 'struct's and 'unions') are the only types
        //:   that might yield a 'true_type' result; all other type categories
        //:   always yield 'false_type'.  Note that we deliberately ignore the
        //:   case of users forging their own implementation of the expanded
        //:   macro; there is no protection against the perfect forger.
        //:
        //: 3 The trait yields 'true_type' only for types that use macros from
        //:   the 'BSLMF_NESTED_TRAIT_DECLARATION' family to associate a trait
        //:   with that class type; all other class types yield 'false_type'.
        //:
        //: 4 A type may be associated with multiple nested traits.
        //:
        //: 5 Two traits in distinct namespaces can unambiguously share a name.
        //:
        //: 6 The trait produces the same result, regardless of cv-qualifiers.
        //:
        //: 7 Nested traits are not inherited.
        //:
        //: 8 "Greedy" types with a conversion-to-anything template operator do
        //:   not support nested traits.
        //
        // Plan:
        //: 1 Verify that the trait under test gives the correct result for
        //:   each category of type:
        //:   o cv-void
        //:   o arithmetic types
        //:   o pointers
        //:   o pointer-to-member-data
        //:   o pointer-to-member-functions
        //:   o references
        //:   o enums
        //:   o arrays
        //:   o class types without nested traits
        //:   o class types with nested traits
        //:   o union types without nested traits
        //:   o union types with nested traits
        //:   o regular function types
        //:   o abominable function types
        //
        // Testing:
        //   DetectNestedTrait::type
        //   DetectNestedTrait::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAIT IS CORRECT METAFUNCTION"
                            "\n=====================================\n");


        typedef InflatableClass InflatableClass::* DataMemberPtrType;
        typedef InflatableClass (InflatableClass::*MethodPtrType)(...) const;

        // C-1, C-2
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(bool,                false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(int,                 false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(long double,         false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(bsl::nullptr_t,      false);

        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(NonflatableEnum,     false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(NonflatableEnumClass,false);
#endif

        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(DataMemberPtrType,   false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(MethodPtrType,       false);

        // 'void' is not an object type, and although it can be cv-qualified,
        // there are no references to 'void' so we must drop to the most
        // primitive test macro:
        ASSERT_DETECT_NESTED_TRAIT(void,                                false);
        ASSERT_DETECT_NESTED_TRAIT(const void,                          false);
        ASSERT_DETECT_NESTED_TRAIT(volatile void,                       false);
        ASSERT_DETECT_NESTED_TRAIT(const volatile void,                 false);

        // Pointers to 'void' are perfectly good object types:
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(void *,              false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(const void *,        false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(volatile void *,     false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(const volatile void *,
                                                                        false);

        // Function types are not object types, nor cv-qualifiable.
        ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(void(...),                  false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(int (float,double),         false);
#if !defined(BSLMF_DETECTNESTEDTRAIT_NO_ABOMINABLE_TYPES)
        ASSERT_DETECT_NESTED_TRAIT(void(...) const,                     false);
        ASSERT_DETECT_NESTED_TRAIT(int (float,double) const,            false);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        ASSERT_DETECT_NESTED_TRAIT(void(...) &,                         false);
        ASSERT_DETECT_NESTED_TRAIT(int (float,double) volatile &&,      false);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        ASSERT_DETECT_NESTED_TRAIT_FOR_TYPE(void(...) noexcept,         false);
        ASSERT_DETECT_NESTED_TRAIT(int (float,double) & noexcept,       false);
        ASSERT_DETECT_NESTED_TRAIT(void(...) const noexcept,            false);
        ASSERT_DETECT_NESTED_TRAIT(int (float,double) const && noexcept,false);
#endif

        // C-2..6

        // class types
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(EmptyClass,          false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(InflatableClass,     true );
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(NonflatableClass,    false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(
                                            Container<InflatableClass>, true );
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(
                                           Container<NonflatableClass>, false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(ExpandedClass,       true );

        // union types
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(EmptyUnion,          false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(InflatableUnion,     true );
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(NonflatableUnion,    false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(
                                            Container<InflatableUnion>, true );
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(
                                           Container<NonflatableUnion>, false);
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(ExpandedUnion,       true );

        // C-7
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(InflatableDerived,   false);

        // C-8 No false-match for greedy types
        ASSERT_DETECT_NESTED_TRAIT_FOR_OBJECT_TYPE(ConvertibleToAny,    false);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //   We will use the test driver trait 'IsInflatable' as a proxy to
        //   indicate the minimal necessary support is available, and also as a
        //   basic test of our test machinery.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Verify the expected value of 'IsInflatable::value' for a variety
        //:   of interesting test types.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using foo::IsInflatable;

        // Direct query of the test trait
        ASSERT( (IsInflatable<InflatableClass>::value));
        ASSERT(!(IsInflatable<NonflatableClass>::value));
        ASSERT( (IsInflatable<Container<InflatableClass> >::value));
        ASSERT(!(IsInflatable<Container<NonflatableClass> >::value));

        ASSERT( (IsInflatable<const InflatableClass>::value));
        ASSERT(!(IsInflatable<const NonflatableClass>::value));
        ASSERT( (IsInflatable<const Container<InflatableClass> >::value));
        ASSERT(!(IsInflatable<const Container<NonflatableClass> >::value));

        ASSERT( (IsInflatable<volatile InflatableClass>::value));
        ASSERT(!(IsInflatable<volatile NonflatableClass>::value));
        ASSERT( (IsInflatable<volatile Container<InflatableClass> >::value));
        ASSERT(!(IsInflatable<volatile Container<NonflatableClass> >::value));

        ASSERT(!(IsInflatable<InflatableClass[1]>::value));
        ASSERT(!(IsInflatable<InflatableClass[]>::value));
        ASSERT(!(IsInflatable<InflatableClass()>::value));

        ASSERT(!(IsInflatable<void>::value));
        ASSERT(!(IsInflatable<int>::value));
        ASSERT(!(IsInflatable<int*>::value));
        ASSERT(!(IsInflatable<int&>::value));

        ASSERT(!(IsInflatable<void(int)>::value));
        ASSERT(!(IsInflatable<int(...)>::value));
        ASSERT(!(IsInflatable<int(&)(int)>::value));
        ASSERT(!(IsInflatable<int(&)(...)>::value));
#if !defined(BSLMF_DETECTNESTEDTRAIT_NO_ABOMINABLE_TYPES)
        ASSERT(!(IsInflatable<int(int) const>::value));
        ASSERT(!(IsInflatable<int(...) const>::value));
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        ASSERT(!(IsInflatable<int(int) noexcept>::value));
        ASSERT(!(IsInflatable<int(...) const & noexcept>::value));
#endif

        ASSERT(!(IsInflatable<ConvertibleToAny>::value));
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
// Copyright 2019 Bloomberg Finance L.P.
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
