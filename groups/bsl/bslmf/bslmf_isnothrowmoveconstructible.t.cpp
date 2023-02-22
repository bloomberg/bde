// bslmf_isnothrowmoveconstructible.t.cpp                             -*-C++-*-
#include <bslmf_isnothrowmoveconstructible.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_isarray.h>      // MSVC workaround, pre-VC 2017
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a metafunction,
// 'bsl::is_nothrow_move_constructible', and a variable template,
// 'bsl::is_nothrow_move_constructible_v', which determine whether a template
// parameter type has a no-throw move constructor.  For C++11 toolchains there
// is a native implementation of this trait that can serve as an oracle for
// testing.
//
// By default, this metafunction supports a restricted set of type categories
// and can be extended to support other types through either template
// specialization or use of the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  The
// native trait and 'bsl' trait should agree for all fundental types and other
// language primitives such as reference types and pointers.  There should also
// be agreement on types that use either of the BDE type trait extensions to
// support this trait for a user type (nested trait declaration, or explicit
// specialization of this trait).
//
// We expect false negatives for the 'bsl' trait where the move constructor is
// marked as 'noexcept' for C++11 and this trait is not explicitly enabled via
// a 'BSLMF_NESTED_TRAIT_DECLARATION' or specialization.  We expect false
// positives in C++03 for non-trivial types that use BDE traits customization
// for the 'is_trivially_copyable' trait, as this serves as a proxy for
// detecting no-throw movability on a wider range of C++03 types.
//
// Thus, we need to ensure that the metafunction correctly identifies natively
// supported types by testing the metafunction with each of the supported type
// categories.  We also need to verify that the metafunction can be correctly
// extended to support user defined types through either of the two supported
// mechanisms; however, extension testing must be done in the
// 'bslmf_movableref' test driver to avoid a cyclic dependency (to expose
// extensions to C++03 environments, 'bslmf::MovableRef' must be used).
// Finally, we need to test correct support for cv-qualified and array types,
// where the underlying type may have a 'noexcept' move constructor.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_nothrow_move_constructible::value
// [ 1] bsl::is_nothrow_move_constructible_v
// ----------------------------------------------------------------------------
// [  ] USAGE EXAMPLE
// [ 2] CONCERN: basic support for class types
// [ 2] CONCERN: traits customization for 'is_nothrow_move_constructible'
// [ 2] CONCERN: traits customization for 'is_trivially_copyable'
// [ 3] QoI: diagnose incomplete types

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
//                      DEFECT DETECTION MACROS
// ----------------------------------------------------------------------------

#define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_NO_NESTED_FOR_ABOMINABLE_FUNCTIONS  1
    // At the moment, 'bsl::is_convertible' will give a hard error when invoked
    // with an abominable function type, as used in 'DetectNestedTrait'.  There
    // is a separate patch coming for this, at which point these tests should
    // be re-enabled.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
# define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_USE_NATIVE_ORACLE     1
    // 'std::is_nothrow_move_constructible' is available as a trusted
    // oracle of the correct value for this trait.
#endif

// ============================================================================
//                      TEST DRIVER CONFIGURATION MACROS
// ----------------------------------------------------------------------------

// Enable the macros below to configure specific builds of the test driver for
// manual testing

//#define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_SHOW_ERROR_FOR_INCOMPLETE_CLASS  1
    // Define this macro to test for compile-fail diagnostics instantiating the
    // 'is_nothrow_move_constructible' trait for an incomplete class type.

//#define BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_SHOW_ERROR_FOR_INCOMPLETE_UNION  1
    // Define this macro to test for compile-fail diagnostics instantiating the
    // 'is_nothrow_move_constructible' trait for an incomplete union type.

//=============================================================================
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below test the 'bsl::is_nothrow_move_constructible'
// trait with a set of variations on a type.  There are several layers of
// macros, as object types support the full range of variation, but function
// types cannot form an array, nor be cv-qualified.  Similarly, 'void' may be
// cv-qualified, but still cannot form an array.  As macros are strictly
// text-substitution we must use the appropriate 'add_decoration' traits to
// transform types in a manner that is guaranteed to be syntactically valid.
// Note that these are not type-dependent contexts, so there is no need to use
// 'typename' when fetching the result from any of the queried traits.  Valid
// entry points into this system of macros are:
//  ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE             : single type
//  ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE        : a type, plus associated
//                                                     pointer/reference types
//  ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE : an object type, plus all
//                                                     reasonable variations

// For the tests below that compare the value of
// 'bsl::is_no_throw_move_constructible' with the "oracle" (i.e.,
// 'std::is_no_throw_move_constructible') adjust the expected value for the
// special cases where the 'bsl' trait intentionally contradicts that provided
// by the 'std' trait.

#if defined(BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_USE_NATIVE_ORACLE)
#if defined(BSLS_PLATFORM_CMP_GNU)                                            \
 && BSLS_PLATFORM_CMP_VERSION >= 110000                                       \
 && BSLS_PLATFORM_CMP_VERSION <  120000                                       \
 && BSLS_COMPILERFEATURES_CPLUSPLUS == 202002L                                \

template <class TYPE>
struct IS_SPECIAL_CASE {
    enum { value =
        (  bsl::is_same<TYPE,                bool                 [128]>::value
        || bsl::is_same<TYPE, const          bool                 [128]>::value
        || bsl::is_same<TYPE,       volatile bool                 [128]>::value
        || bsl::is_same<TYPE, const volatile bool                 [128]>::value

        || bsl::is_same<TYPE,                void*                [128]>::value
        || bsl::is_same<TYPE, const          void*                [128]>::value
        || bsl::is_same<TYPE, const          void* const          [128]>::value
        || bsl::is_same<TYPE,       volatile void*                [128]>::value
        || bsl::is_same<TYPE,       volatile void*       volatile [128]>::value
        || bsl::is_same<TYPE, const volatile void*                [128]>::value
        || bsl::is_same<TYPE, const volatile void* const          [128]>::value
        || bsl::is_same<TYPE, const volatile void*       volatile [128]>::value
        || bsl::is_same<TYPE, const volatile void* const volatile [128]>::value
        ) };
};

#define ORACLE_VALUE(TYPE) (std::is_nothrow_move_constructible<TYPE>::value   \
                            && !IS_SPECIAL_CASE<TYPE>::value)
#else
#define ORACLE_VALUE(TYPE) std::is_nothrow_move_constructible<TYPE>::value
#endif
#else
    // Cannot use oracle if is not available.
#endif

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE
//   This macro validates that the 'bsl' trait has the same result as the
//   native trait if it is available, and expands to nothing otherwise.
#if defined(BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_USE_NATIVE_ORACLE)
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE)            \
    ASSERTV(                                                                  \
       ORACLE_VALUE(TYPE),   bsl::is_nothrow_move_constructible<TYPE>::value, \
       ORACLE_VALUE(TYPE) == bsl::is_nothrow_move_constructible<TYPE>::value);\
    // Confirm that the result of 'bsl::is_nothrow_move_constructible<TYPE>'
    // agrees with the oracle 'std::is_nothrow_move_constructible'.
#else
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE)
    // The native trait is not available to act as an oracle, so there is no
    // test to perform.
#endif

// Macro: ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT
//   This macro validates that the 'bsl' variable template has the same value
//   as the associated trait, if variable templates are supported by the
//   compiler, and expands to nothing otherwise.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
# define ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)                         \
    ASSERT( bsl::is_nothrow_move_constructible  <TYPE>::value ==              \
            bsl::is_nothrow_move_constructible_v<TYPE>)
    // 'ASSERT' that 'is_nothrow_move_constructible_v' has the same value as
    // 'is_nothrow_move_constructible::value'.
#else
# define ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)
#endif

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for the given 'TYPE', and that all manifestations of that trait
//   and value are consistent.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE, RESULT)                    \
    ASSERT( bsl::is_nothrow_move_constructible<TYPE>::value ==  RESULT);      \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE);                \
    ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)
    // Test that the result of 'bsl::is_nothrow_move_constructible<TYPE>' has
    // the same value as the expected 'RESULT'.  Confirm that the expected
    // result agrees with the native oracle, where available.  Finally. confirm
    // that the associated variable template, when available, has a value that
    // agrees with this trait instantiation.

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for an rvalue reference to the given 'TYPE' on platforms that
//   implement language support, and performs no test otherwise.  Note that the
//   native trait implementation shipping with Visual C++ compilers prior to
//   MSVC 2017 erroneously reports that rvalue-references to arrays are NOT
//   no-throw move constructible.
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1910
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)          \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                      bsl::add_rvalue_reference<TYPE>::type,  \
                                      !bsl::is_array<TYPE>::value)
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)          \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                bsl::add_rvalue_reference<TYPE>::type, true)
#else
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)
#endif

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for the given 'TYPE', and pointers/references to that type.
//   Pointers and references are always no-throw move constructible.
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT)              \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE, RESULT);                       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(bsl::add_pointer<TYPE>::type, true); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                bsl::add_lvalue_reference<TYPE>::type, true); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for all cv-qualified variations of the given 'TYPE', and pointers
//   and references to each of those cv-qualified types.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)            \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_cv<TYPE>::type, RESULT)

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for a given 'TYPE' and all cv-qualified variations of that given
//   'TYPE', and pointers and references to each of those cv-qualified types.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE(TYPE, RESULT)   \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT);                  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT);

// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for the given object 'TYPE', and for pointers, references to, and
//   arrays of that type, and the expected 'CVRESULT' for all cv-qualified
//   variations of that type, and for pointers, references to, and arrays of
//   each of those cv-qualified types.  Note that this macro does not
//   recursively test arrays of pointers to 'TYPE'.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(TYPE,                \
                                                         RESULT,              \
                                                         CVRESULT)            \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT);                  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, CVRESULT);             \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE(TYPE[128], false);  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE(TYPE[9][8], false); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE(TYPE[], false);     \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_AND_CV_TYPE(TYPE[][8], false)


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This 'enum' type is used for testing.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum EnumClassType {
    // This 'enum' type is used for testing.
};
#endif

struct UserDefinedTestType {
    // This user-defined type is used for testing.

    // CREATORS
    UserDefinedTestType()
        // Create a default 'UserDefinedTestType' object.
    {
    }
};

typedef int UserDefinedTestType::*DataMemberPtrTestType;
    // This pointer to instance data member type is used for testing.

typedef int (UserDefinedTestType::*MethodPtrTestType)();
    // This pointer to non-static function member type is used for testing.

}  // close unnamed namespace

struct Incomplete;  // Incomplete class type for testing
union  Uncomplete;  // Incomplete union type for testing

struct NonTrivial {
    // This 'struct' has non-trivial constructors and no trait customizations,
    // so is clearly NOT no-throw move constructible in any dialect of C++.

    // CREATORS
    NonTrivial() {}
    NonTrivial(const NonTrivial&) {}
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
struct NonTrivialWithMoveConstructor {
    // This 'struct' has a non-'noexcept' move constructor, so is not no-throw
    // move constructible in any dialect of C++.

    // CREATORS
    NonTrivialWithMoveConstructor(NonTrivialWithMoveConstructor&&)
        // Create an instance of the object using move construction.
    {
    }
};

struct NonTrivialWithNoexceptMoveConstructor {
    // This 'struct' has a 'noexcept' move constructor, so is no-throw move
    // constructible in any dialect of C++ that supports 'noexcept'.

    // CREATORS
    NonTrivialWithNoexceptMoveConstructor(
                 NonTrivialWithNoexceptMoveConstructor&&) BSLS_KEYWORD_NOEXCEPT
        // Create an instance of the object using move construction.
    {
    }
};
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

struct NonTrivialWithMovableRefConstructor {
    // This 'struct' has a non-'noexcept' 'MovableRef' move constructor, so is
    // not no-throw move constructible in any dialect of C++.

    // CREATORS
    NonTrivialWithMovableRefConstructor(
                        bslmf::MovableRef<NonTrivialWithMovableRefConstructor>)
        // Create an instance of the object using move construction.
    {
    }
};

struct NonTrivialWithNoexceptMovableRefConstructor {
    // This 'struct' has a 'noexcept' 'MovableRef' move constructor, so is
    // no-throw move constructible in any dialect of C++ that supports
    // 'noexcept'.

    // CREATORS
    NonTrivialWithNoexceptMovableRefConstructor(
                bslmf::MovableRef<NonTrivialWithNoexceptMovableRefConstructor>)
        BSLS_KEYWORD_NOEXCEPT
        // Create an instance of the object using move construction.
    {
    }
};

union NonTrivialUnion {
    // This 'struct' has non-trivial constructors and no trait customizations,
    // so is clearly NOT no-throw move constructible in any dialect of C++.

    // CREATORS
    NonTrivialUnion() {}
    NonTrivialUnion(const NonTrivialUnion&) {}
};

struct ClassUsingNestedCopyable {
    // This 'struct', which is marked to have a 'nothrow' move constructor
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(ClassUsingNestedCopyable,
                                   bsl::is_trivially_copyable);
};

struct ClassUsingNestedMovable {
    // This 'struct', which is marked to have a 'nothrow' move constructor
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(ClassUsingNestedMovable,
                                   bsl::is_nothrow_move_constructible);
};

union UnionUsingNestedCopyable {
    // This 'struct', which is marked to have a 'nothrow' move constructor
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UnionUsingNestedCopyable,
                                   bsl::is_trivially_copyable);
};

union UnionUsingNestedMovable {
    // This 'struct', which is marked to have a 'nothrow' move constructor
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UnionUsingNestedMovable,
                                   bsl::is_nothrow_move_constructible);
};

struct NothrowMovableClass {
    // This empty 'struct' indicates it can be moved without risk of throwing
    // to C++03 toolchains by explicitly specializing the
    // 'bsl::is_nothrow_move_constructible' trait.
};

union NothrowMovableUnion {
    // This empty 'union' indicates it can be moved without risk of throwing to
    // C++03 toolchains by explicitly specializing the
    // 'bsl::is_nothrow_move_constructible' trait.
};

struct TrivialClass {
    // This empty 'struct' indicates its triviality to C++03 toolchains by
    // explicitly specializing the 'bsl::is_trivially_copyable' trait.
};

union TrivialUnion {
    // This empty 'union' indicates its triviality to C++03 toolchains by
    // explicitly specializing the 'bsl::is_trivially_copyable' trait.
};

namespace bsl {
template <>
struct is_trivially_copyable<TrivialClass> : bsl::true_type {};
template <>
struct is_trivially_copyable<TrivialUnion> : bsl::true_type {};

template <>
struct is_nothrow_move_constructible<NothrowMovableClass> : bsl::true_type {};
template <>
struct is_nothrow_move_constructible<NothrowMovableUnion> : bsl::true_type {};
}  // close namespace bsl

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES
        //  It is not possible to determine whether an incomplete class type is
        //  no-throw move constructible as we may produce different results
        //  once the type is complete, leading to difficult to diagnose ODR
        //  violations.
        //
        // Concerns:
        //:  1 Instantiating the 'bsl::is_nothrow_move_constructible' is
        //:    clearly diagnosed with an error that cannot be circumvented by
        //:    template tricks exploiting SFINAE.
        //:
        //:  2 Pointers and references to incomplete types are perfectly valid
        //:    types for which the trait returns 'true'.
        //
        // Plan:
        //:  1 Under a pre-processor check requiring manual enablement, try to
        //:    instantiate 'bsl::is_nothrow_move_constructible' for incomplete
        //:    class types.
        //:
        //:  2 Verify that pointers and references to incomplete class types
        //:    return 'true' for this trait.
        //
        // Testing:
        //   QoI: diagnose incomplete types
        // --------------------------------------------------------------------

        if (verbose)
               printf("\nTESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES"
                      "\n=================================================\n");

        // C-1
#if defined(BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_SHOW_ERROR_FOR_INCOMPLETE_CLASS)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT( bsl::is_nothrow_move_constructible<Incomplete>::value);
#endif

#if defined(BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_SHOW_ERROR_FOR_INCOMPLETE_UNION)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT( bsl::is_nothrow_move_constructible<Uncomplete>::value);
#endif

        // C-2
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(Incomplete *,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(Uncomplete *,
                                                         true,
                                                         true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(Incomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const Incomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(volatile Incomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const volatile Incomplete&, true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(Uncomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const Uncomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(volatile Uncomplete&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const volatile Uncomplete&, true);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(Incomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const Incomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(volatile Incomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const volatile Incomplete&&,true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(Uncomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const Uncomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(volatile Uncomplete&&, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const volatile Uncomplete&&,true);
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC SUPPORT FOR CLASS TYPES
        //   Ensure the 'bsl::is_nothrow_move_constructible' metafunction
        //   returns 'false' for unassociated classes, and 'true' otherwise.
        //   Note that there are distinct concerns for C++03/11 disagreeing
        //   when classes can formally declare a 'noexcept' move constructor
        //   that also impact cv-qualifier concerns, and we defer such testing
        //   to the 'bslmf_movableref' test driver, which suppplies the tools
        //   to declare move constructors in both C++03/11.  Therefore, this
        //   test case will validate 'true' results for only cv-unqualified
        //   class types.
        //
        // Concerns:
        //:  1 The metafunction returns 'false' for cv-qualified non-trivial
        //:    class types.
        //:
        //:  2 For classes with user-defined move constructors, where
        //:    'noexcept' specifications are supported, the metafunction
        //:     respects the 'noexcept' specification of the move constructor,
        //:     and returns 'false' otherwise.
        //:
        //:  3 The metafunction returns 'true' for class types that associate
        //:    this trait using either of the BDE traits association
        //:    techniques.
        //
        // Plan:
        //:  1 Verify that a non-trivial class type is a regular object type
        //:    returning 'false' for this trait.
        //:
        //:  2 Verify that a class type with a move constructor returns 'false'
        //:    for this trait unless the move constructor is callable and has a
        //:    valid 'noexcept(true)' specification, in which case it returns
        //:    'true'.
        //:
        //:  3 Verify that trivial class returns 'true' for this trait when
        //:    using either of the BDE traits association techniques.   Note
        //:    that the types should be trivial to preserve correct behavior
        //:    with the C++11 native oracle.
        //:
        //:  4 Defer all other testing of trait behavior with class types to
        //:    the 'bslmf_movableref' test driver.
        //
        // Testing:
        //   CONCERN: basic support for class types
        //   CONCERN: traits customization for 'is_nothrow_move_constructible'
        //   CONCERN: traits customization for 'is_trivially_copyable'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC SUPPORT FOR CLASS TYPES"
                            "\n=====================================\n");

        // C-1
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(NonTrivial,
                                                         false,
                                                         false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(NonTrivialUnion,
                                                         false,
                                                         false);

        // C-2
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        // The move constructor is non-'noexcept' so we expect 'false'.
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                   NonTrivialWithMoveConstructor,
                                   false,
                                   false);

        // The move constructor in the test class is not usable for
        // cv-qualified types, so we expect the trait to return 'false'.  Move
        // construction is no-throw only if 'noexcept' is supported.
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                   NonTrivialWithNoexceptMoveConstructor,
                                   BSLS_KEYWORD_NOEXCEPT_AVAILABLE,
                                   false);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // The move constructor is non-'noexcept' so we expect 'false'.
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                   NonTrivialWithMovableRefConstructor,
                                   false,
                                   false);

        // The move constructor in the test class is not usable for
        // cv-qualified types, so we expect the trait to return 'false'.  Move
        // construction is no-throw only if 'noexcept' is supported.
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                   NonTrivialWithNoexceptMovableRefConstructor,
                                   BSLS_KEYWORD_NOEXCEPT_AVAILABLE,
                                   false);

        // C-3
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedCopyable, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedMovable,  true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedCopyable, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedMovable,  true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedCopyable[],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedMovable[],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedCopyable[],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedMovable[],
                                             false);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedCopyable[1],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(ClassUsingNestedMovable[2],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedCopyable[3],
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(UnionUsingNestedMovable[4],
                                             false);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableClass,    true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableUnion,    true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialClass,           true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialUnion,           true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableClass[],  false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableUnion[],  false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialClass[],         false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialUnion[],         false);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableClass[1], false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(NothrowMovableUnion[2], false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialClass[3],        false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TrivialUnion[4],        false);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_nothrow_move_constructible::value'
        //   Ensure the 'bsl::is_nothrow_move_constructible' metafunction
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The metafunction returns 'true' for fundamental types.
        //:
        //:  2 The metafunction returns 'true' for enumerated types.
        //:
        //:  3 The metafunction returns 'true' for pointer to member types.
        //:
        //:  4 The metafunction returns 'false' for cv-qualified 'void' types.
        //:
        //:  5 The metafunction returns 'false' for function types.
        //:
        //:  6 The metafunction returns 'true' for pointer types.
        //:
        //:  7 The metafunction returns 'false' for reference types.
        //:
        //:  8 The metafunction returns the same result for array types as it
        //:    would for the array's element type.
        //:
        //:  9 The metafunction returns the same result for cv-qualified
        //:    types that it would return 'true' for the corresponding
        //:    cv-unqualified type.
        //:
        //: 10 That 'is_nothrow_move_constructible<T>::value' has the same
        //:    value as 'is_nothrow_move_constructible_v<T>' for a variety of
        //:    template parameter types.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C6-9)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-5, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   bsl::is_nothrow_move_constructible::value
        //   bsl::is_nothrow_move_constructible_v
        // --------------------------------------------------------------------

        if (verbose)
             printf("\nTESTING 'bsl::is_nothrow_move_constructible::value'"
                    "\n===================================================\n");

        // C-1
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(bool, true, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(char, true, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(char8_t, true, true);
#endif
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(int, true, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(long double,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(bsl::nullptr_t,
                                                         true,
                                                         true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(char16_t, true, true);
#endif

        // C-2
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(EnumTestType,
                                                         true,
                                                         true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(EnumClassType,
                                                         true,
                                                         true);
#endif

        // C-3
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(DataMemberPtrTestType,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(MethodPtrTestType,
                                                         true,
                                                         true);

        // C-4 : 'void' is not an object type, and although it can be
        // cv-qualified, there are no references to 'void' so we must drop to
        // the most primitive test macro:
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(void,                false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const void,          false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(volatile void,       false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(const volatile void, false);

        // Pointers to void are perfectly good object types:
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(void *,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(const void *,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(volatile void *,
                                                         true,
                                                         true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(const volatile void *,
                                                         true,
                                                         true);

        // C-5 : Function types are not object types, nor cv-qualifiable.
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(void(),               false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(int(float,double...), false);
#ifndef BSLMF_ISNOTHROWMOVECONSTRUCTIBLE_NO_NESTED_FOR_ABOMINABLE_FUNCTIONS
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(void() const,              false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(int(float,double...) const,false);
#endif
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
