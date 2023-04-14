// bslmf_isnothrowswappable.t.cpp                                     -*-C++-*-
#include <bslmf_isnothrowswappable.h>

#include <bsla_maybeunused.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_isarray.h>      // MSVC workaround, pre-VC 2017
#include <bslmf_movableref.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
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
// 'bsl::is_nothrow_swappable', and a variable template,
// 'bsl::is_nothrow_swappable_v', which determine whether, after a call to
// 'using std::swap;', the operation 'swap(x,y)' is valid where x and y are of
// the type specified in the template parameter.  For C++17 toolchains there is
// a native implementation of this trait that can serve as an oracle for
// testing.
//
// Note that this metafunction is not implemented for C++03.
//
// This metafunction can be extended to support other types through template
// specialization (the use of the BDE-style trait declarations is not supported
// for this purpose).
//
// Thus, we need to ensure that the metafunction correctly identifies natively
// supported types by testing the metafunction with each of the supported type
// categories.  We also need to verify that the metafunction can be correctly
// extended to support user defined types through the supported template
// specialization mechanism.  Finally, we need to test correct support for
// cv-qualified and array types, where the underlying type is nothrow
// swappable.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_nothrow_swappable::value
// [ 1] bsl::is_nothrow_swappable_v
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] CONCERN: basic support for class types
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
# define BSLMF_ISNOTHROWSWAPPABLE_USE_NATIVE_ORACLE     1
    // 'std::is_nothrow_swappable' is available as a trusted oracle of the
    // correct value for this trait.
#endif


#if defined(BSLS_PLATFORM_CMP_MSVC) &&                                        \
    ((defined(BSLS_COMPILERFEATURES_CPLUSPLUS) &&                             \
      (BSLS_COMPILERFEATURES_CPLUSPLUS < 201703L)) ||                         \
     (defined(BSLS_PLATFORM_CMP_VERSION) &&                                   \
      (BSLS_PLATFORM_CMP_VERSION < 1920)))
# define BSLMF_MSVC_ISNOTHROWSWAPPABLE_ALWAYS_FALSE     1
    // For MSVC 2017 and all MSVC versions building C++14 (C++14 is the lowest
    // supported by MSVC), a combination of MSVC's name lookup and sfinae
    // peculiarities means we cannot reproduce this behaviour.  Under these
    // unfortunate circumstances, it is therefore safest to assume nothing is
    // `nothrow` swappable.
#endif

// ============================================================================
//                      TEST DRIVER CONFIGURATION MACROS
// ----------------------------------------------------------------------------

// Enable the macros below to configure specific builds of the test driver for
// manual testing

//#define BSLMF_ISNOTHROWSWAPPABLE_SHOW_ERROR_FOR_INCOMPLETE_CLASS  1
    // Define this macro to test for compile-fail diagnostics instantiating the
    // 'is_nothrow_swappable' trait for an incomplete class type.

//#define BSLMF_ISNOTHROWSWAPPABLE_SHOW_ERROR_FOR_INCOMPLETE_UNION  1
    // Define this macro to test for compile-fail diagnostics instantiating the
    // 'is_nothrow_swappable' trait for an incomplete union type.

//=============================================================================
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below test the 'bsl::is_nothrow_swappable' trait with a
// set of variations on a type.  There are several layers of macros, as object
// types support the full range of variation, but function types cannot form an
// array, nor be cv-qualified.  Similarly, 'void' may be cv-qualified, but
// still cannot form an array.  As macros are strictly text-substitution we
// must use the appropriate 'add_decoration' traits to transform types in a
// manner that is guaranteed to be syntactically valid.
//
// Note that these are not type-dependent contexts, so there is no need to use
// 'typename' when fetching the result from any of the queried traits.  Valid
// entry points into this system of macros are:
//
//  ASSERT_IS_NOTHROW_SWAPPABLE             : single type
//  ASSERT_IS_NOTHROW_SWAPPABLE_TYPE        : a type, plus associated
//                                                     pointer/reference types
//  ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE : an object type, plus all
//                                                     reasonable variations
//

// Macro: ASSERT_IS_NOTHROW_SWAPPABLE_CONSULT_ORACLE
//   This macro validates that the 'bsl' trait has the same result as the
//   native trait if it is available, and expands to nothing otherwise.
#if defined(BSLMF_ISNOTHROWSWAPPABLE_USE_NATIVE_ORACLE)
#define ASSERT_IS_NOTHROW_SWAPPABLE_CONSULT_ORACLE(TYPE)                      \
    ASSERTV(                                                                  \
           std::is_nothrow_swappable<TYPE>::value,                            \
           bsl::is_nothrow_swappable<TYPE>::value,                            \
           (std::is_nothrow_swappable<TYPE>::value ==                         \
            bsl::is_nothrow_swappable<TYPE>::value))
    // Confirm that the result of 'bsl::is_nothrow_swappable<TYPE>' agrees with
    // the oracle 'std::is_nothrow_swappable'.
#else
# define ASSERT_IS_NOTHROW_SWAPPABLE_CONSULT_ORACLE(TYPE)
    // The native trait is not available to act as an oracle, so there is no
    // test to perform.
#endif


// Macro: ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT
//   This macro validates that the 'bsl' variable template has the same value
//   as the associated trait, if variable templates are supported by the
//   compiler, and expands to nothing otherwise.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
#define ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)                          \
    ASSERT(bsl::is_nothrow_swappable<TYPE>::value ==                          \
           bsl::is_nothrow_swappable_v<TYPE>)
    // 'ASSERT' that 'is_nothrow_swappable_v' has the same value as
    // 'is_nothrow_swappable::value'.
#else
# define ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)
#endif


// Macro: ASSERT_IS_NOTHROW_SWAPPABLE
//   This macro tests that the 'is_nothrow_swappable' trait has the expected
//   'RESULT' for the given 'TYPE', and that all manifestations of that trait
//   and value are consistent.  For C++03 the trait is not implemented so we do
//   not perform the test at all.  For MSVC we have an implementation that
//   deliberately always returns false for C++14.
#if defined(BSLMF_MSVC_ISNOTHROWSWAPPABLE_ALWAYS_FALSE)

#define ASSERT_IS_NOTHROW_SWAPPABLE(TYPE, RESULT)                             \
    ASSERT(bsl::is_nothrow_swappable<TYPE>::value == false);
    // For MSVC versions for which the trait is defined to be always false,
    // test that the trait is false.

#elif defined(BSLS_COMPILERFEATURES_CPLUSPLUS) &&                             \
    BSLS_COMPILERFEATURES_CPLUSPLUS == 199711L

#define ASSERT_IS_NOTHROW_SWAPPABLE(TYPE, RESULT) ASSERT(true);
    // For C++03, the trait is not defined so skip the test.

#else

#define ASSERT_IS_NOTHROW_SWAPPABLE(TYPE, RESULT)                             \
    ASSERT(bsl::is_nothrow_swappable<TYPE>::value == RESULT);                 \
    ASSERT_IS_NOTHROW_SWAPPABLE_CONSULT_ORACLE(TYPE);                         \
    ASSERT_VARIABLE_TEMPLATE_IS_CONSISTENT(TYPE)
    // Test that the result of 'bsl::is_nothrow_swappable<TYPE>' has the same
    // value as the expected 'RESULT'.  Confirm that the expected result agrees
    // with the native oracle, where available.  Finally. confirm that the
    // associated variable template, when available, has a value that agrees
    // with this trait instantiation.
#endif


// Macro: ASSERT_IS_NOTHROW_SWAPPABLE_RVAL_REF
//   This macro tests that the 'is_nothrow_swappable' trait has the expected
//   'RESULT' for an rvalue reference to the given 'TYPE' on platforms that
//   implement language support, and performs no test otherwise.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define ASSERT_IS_NOTHROW_SWAPPABLE_RVAL_REF(TYPE, RESULT)                    \
    ASSERT_IS_NOTHROW_SWAPPABLE(bsl::add_rvalue_reference<TYPE>::type,        \
                                RESULT)
#else
# define ASSERT_IS_NOTHROW_SWAPPABLE_RVAL_REF(TYPE, RESULT)
#endif


// Macro: ASSERT_IS_NOTHROW_SWAPPABLE_TYPE
//   This macro tests that the 'is_nothrow_swappable' trait has the expected
//   'RESULT' for the given 'TYPE', and pointers/references to that type.
//   Pointers are always no-throw swappable.
#define ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(TYPE, RESULT)                        \
    ASSERT_IS_NOTHROW_SWAPPABLE(TYPE, RESULT);                                \
    ASSERT_IS_NOTHROW_SWAPPABLE(bsl::add_pointer<TYPE>::type, true);          \
    ASSERT_IS_NOTHROW_SWAPPABLE(bsl::add_lvalue_reference<TYPE>::type,        \
                                RESULT);                                      \
    ASSERT_IS_NOTHROW_SWAPPABLE_RVAL_REF(TYPE, RESULT)


// Macro: ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE
//   This macro tests that the 'is_nothrow_swappable' trait has the expected
//   'RESULT' for the given 'TYPE', and all cv-qualified variations of that
//   type, and pointers and references to each of those cv-qualified types.
//   const-qualified types are never nothrow swappable.
#define ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE, RESULT, VRESULT)            \
    ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(TYPE, RESULT);                           \
    ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(bsl::add_const<TYPE>::type, false);      \
    ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(bsl::add_volatile<TYPE>::type, VRESULT); \
    ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(bsl::add_cv<TYPE>::type, false)


// Macro: ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE
//   This macro tests that the 'is_nothrow_swappable' trait has the expected
//   'RESULT' for the given object 'TYPE', and all cv-qualified variations of
//   that type, and pointers and references to each of those cv-qualified
//   types, and arrays of those cv-qualified types.  Note that this macro does
//   not recursively test arrays of pointers to 'TYPE'.
#define ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(TYPE, RESULT, VRESULT)        \
    ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE, RESULT, VRESULT);               \
    ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE[128], RESULT, VRESULT);          \
    ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE[12][8], RESULT, VRESULT);        \
    ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE[], false, false);                \
    ASSERT_IS_NOTHROW_SWAPPABLE_CV_TYPE(TYPE[][8], false, false)


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


struct Incomplete;  // Incomplete class type for testing
union  Uncomplete;  // Incomplete union type for testing

struct NonTrivialClass {
    // This 'struct' has non-trivial constructors so is NOT no-throw swappable.

    // CREATORS
    NonTrivialClass() {}
        // Construct a test object.
    NonTrivialClass(const NonTrivialClass&) {}
        // Copy construct a test object.
};

union NonTrivialUnion {
    // This 'union' has non-trivial constructors so is NOT no-throw swappable.

    // CREATORS
    NonTrivialUnion() {}
        // Construct a test object.
    NonTrivialUnion(const NonTrivialUnion&) {}
        // Copy construct a test object.
};

struct NonTrivialClassWithFreeSwap {
    // This empty 'struct' indicates it can be swapped but not nothrow swapped
    // by explicitly creating a free 'swap' function.

    // CREATORS
    NonTrivialClassWithFreeSwap() {}
        // Construct a test object.
    NonTrivialClassWithFreeSwap(const NonTrivialClassWithFreeSwap&) {}
        // Copy construct a test object.
};

struct NonTrivialUnionWithFreeSwap {
    // This empty 'union' indicates it can be swapped but not nothrow swapped
    // by explicitly creating a free 'swap' function.

    // CREATORS
    NonTrivialUnionWithFreeSwap() {}
        // Construct a test object.
    NonTrivialUnionWithFreeSwap(const NonTrivialUnionWithFreeSwap&) {}
        // Copy construct a test object.
};

BSLA_MAYBE_UNUSED
void swap(NonTrivialClassWithFreeSwap&, NonTrivialClassWithFreeSwap&) {}
BSLA_MAYBE_UNUSED
void swap(NonTrivialUnionWithFreeSwap&, NonTrivialUnionWithFreeSwap&) {}

struct NonTrivialClassWithFreeNoexceptSwap {
    // This empty 'struct' indicates it can be swapped without risk of throwing
    // by explicitly creating a free 'noexcept' 'swap' function.

    // CREATORS
    NonTrivialClassWithFreeNoexceptSwap() {}
        // Construct a test object.
    NonTrivialClassWithFreeNoexceptSwap(
                                    const NonTrivialClassWithFreeNoexceptSwap&)
        // Copy construct a test object.
    {
    }
};

struct NonTrivialUnionWithFreeNoexceptSwap {
    // This empty 'struct' indicates it can be swapped without risk of throwing
    // by explicitly creating a free 'noexcept' 'swap' function.

    // CREATORS
    NonTrivialUnionWithFreeNoexceptSwap() {}
        // Construct a test object.
    NonTrivialUnionWithFreeNoexceptSwap(
                                    const NonTrivialUnionWithFreeNoexceptSwap&)
        // Copy construct a test object.
    {
    }
};

BSLA_MAYBE_UNUSED
void swap(NonTrivialClassWithFreeNoexceptSwap&,
          NonTrivialClassWithFreeNoexceptSwap&) BSLS_KEYWORD_NOEXCEPT
    // Dummy swap function to enable nothrow swappability for
    // 'NonTrivialClassWithFreeNoexceptSwap'.
{
}
BSLA_MAYBE_UNUSED
void swap(NonTrivialUnionWithFreeNoexceptSwap&,
          NonTrivialUnionWithFreeNoexceptSwap&) BSLS_KEYWORD_NOEXCEPT
    // Dummy swap function to enable nothrow swappability for
    // 'NonTrivialUnionWithFreeNoexceptSwap'.
{
}

struct NonTrivialMovableAssignableClass {
    // This empty 'struct' indicates it can be swapped but not nothrow swapped
    // by having both a move and an assignment operator.

    // CREATORS
    NonTrivialMovableAssignableClass() {}
        // Construct a test object.
    NonTrivialMovableAssignableClass(const NonTrivialMovableAssignableClass&)
        // Copy construct a test object.
    {
    }
    NonTrivialMovableAssignableClass(
                           bslmf::MovableRef<NonTrivialMovableAssignableClass>)
        // Move construct a test object.
    {
    }

    // MANIPULATORS
    BSLA_MAYBE_UNUSED NonTrivialMovableAssignableClass& operator=(
                                      const NonTrivialMovableAssignableClass&);
        // Move assign a test object.
};

NonTrivialMovableAssignableClass& NonTrivialMovableAssignableClass::operator=(
                                       const NonTrivialMovableAssignableClass&)
{
    return *this;
}

struct NonTrivialNoexceptMovableAssignableClass {
    // This empty 'struct' indicates it can be swapped but not nothrow swapped
    // by having both a move and an assignment operator, where only move is
    // noexcept.

    // CREATORS
    NonTrivialNoexceptMovableAssignableClass() {}
        // Construct a test object.
    NonTrivialNoexceptMovableAssignableClass(
                               const NonTrivialNoexceptMovableAssignableClass&)
        // Copy construct a test object.
    {
    }
    NonTrivialNoexceptMovableAssignableClass(
                   bslmf::MovableRef<NonTrivialNoexceptMovableAssignableClass>)
        BSLS_KEYWORD_NOEXCEPT
        // Move construct a test object.
    {
    }

    // MANIPULATORS
    BSLA_MAYBE_UNUSED NonTrivialNoexceptMovableAssignableClass& operator=(
                              const NonTrivialNoexceptMovableAssignableClass&);
        // Assign a test object.
};

NonTrivialNoexceptMovableAssignableClass&
NonTrivialNoexceptMovableAssignableClass::operator=(
                               const NonTrivialNoexceptMovableAssignableClass&)
{
    return *this;
}

struct NonTrivialMovableNoexceptAssignableClass {
    // This empty 'struct' indicates it can be swapped but not nothrow swapped
    // by having both a move and an assignment operator, where only assignment
    // is noexcept.

    // CREATORS
    NonTrivialMovableNoexceptAssignableClass() {}
        // Construct a test object.
    NonTrivialMovableNoexceptAssignableClass(
                               const NonTrivialMovableNoexceptAssignableClass&)
        // Copy construct a test object.
    {
    }
    NonTrivialMovableNoexceptAssignableClass(
                   bslmf::MovableRef<NonTrivialMovableNoexceptAssignableClass>)
        // Move construct a test object.
    {
    }

    // MANIPULATORS
    BSLA_MAYBE_UNUSED NonTrivialMovableNoexceptAssignableClass& operator=(
        const NonTrivialMovableNoexceptAssignableClass&) BSLS_KEYWORD_NOEXCEPT;
        // Assign a test object.
};

NonTrivialMovableNoexceptAssignableClass&
NonTrivialMovableNoexceptAssignableClass::operator=(
         const NonTrivialMovableNoexceptAssignableClass&) BSLS_KEYWORD_NOEXCEPT
{
    return *this;
}

struct NonTrivialNoexceptMovableNoexceptAssignableClass {
    // This empty 'struct' indicates it can be swapped and nothrow swapped by
    // having both a move and an assignment operator, where both are noexcept.

    // CREATORS
    NonTrivialNoexceptMovableNoexceptAssignableClass() {}
        // Construct a test object.
    NonTrivialNoexceptMovableNoexceptAssignableClass(
                       const NonTrivialNoexceptMovableNoexceptAssignableClass&)
        // Copy construct a test object.
    {
    }
    NonTrivialNoexceptMovableNoexceptAssignableClass(
           bslmf::MovableRef<NonTrivialNoexceptMovableNoexceptAssignableClass>)
        BSLS_KEYWORD_NOEXCEPT
        // Move construct a test object.
    {
    }

    // MANIPULATORS
    BSLA_MAYBE_UNUSED NonTrivialNoexceptMovableNoexceptAssignableClass&
    operator=(const NonTrivialNoexceptMovableNoexceptAssignableClass&)
        BSLS_KEYWORD_NOEXCEPT;
        // Assign a test object.
};

NonTrivialNoexceptMovableNoexceptAssignableClass&
NonTrivialNoexceptMovableNoexceptAssignableClass::operator=(
 const NonTrivialNoexceptMovableNoexceptAssignableClass&) BSLS_KEYWORD_NOEXCEPT
{
    return *this;
}

struct BSLA_MAYBE_UNUSED NonTrivialPrivateMovePrivateAssignment {
    // This empty 'struct' indicates it is not swappable by making the move and
    // assignment operators private.

    // CREATORS
    NonTrivialPrivateMovePrivateAssignment() {}
        // Construct a test object.
    NonTrivialPrivateMovePrivateAssignment(
                                 const NonTrivialPrivateMovePrivateAssignment&)
        // Copy construct a test object.
    {
    }

  private:
    // PRIVATE CREATORS
    NonTrivialPrivateMovePrivateAssignment(
                    bslmf::MovableRef<NonTrivialPrivateMovePrivateAssignment>);
        // Move construct a test object.

    // MANIPULATORS
    BSLA_MAYBE_UNUSED NonTrivialPrivateMovePrivateAssignment& operator=(
                                const NonTrivialPrivateMovePrivateAssignment&);
        // Assign a test object.
};

NonTrivialPrivateMovePrivateAssignment&
NonTrivialPrivateMovePrivateAssignment::operator=(
                                 const NonTrivialPrivateMovePrivateAssignment&)
{
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
struct NonTrivialDeletedMoveDeletedAssignmentClass {
    // This empty 'struct' indicates it is not swappable by deleting the move
    // and assignment operators (where supported in the variant of C++).

    // CREATORS
    NonTrivialDeletedMoveDeletedAssignmentClass() {}
        // Construct a test object.
    NonTrivialDeletedMoveDeletedAssignmentClass(
                            const NonTrivialDeletedMoveDeletedAssignmentClass&)
        // Copy construct a test object.
    {
    }

    NonTrivialDeletedMoveDeletedAssignmentClass(
                       NonTrivialDeletedMoveDeletedAssignmentClass&&) = delete;
        // Move construct a test object.

    // MANIPULATORS
    NonTrivialDeletedMoveDeletedAssignmentClass& operator=(
                  const NonTrivialDeletedMoveDeletedAssignmentClass&) = delete;
        // Assign a test object.
};

struct NonTrivialMovableAssignableDeletedSwapClass {
    // This empty 'struct' indicates it is not swappable by deleting the free
    // 'swap' function.

    // CREATORS
    NonTrivialMovableAssignableDeletedSwapClass() {}
        // Construct a test object.
    NonTrivialMovableAssignableDeletedSwapClass(
                            const NonTrivialMovableAssignableDeletedSwapClass&)
        // Copy construct a test object.
    {
    }
    NonTrivialMovableAssignableDeletedSwapClass(
                                 NonTrivialMovableAssignableDeletedSwapClass&&)
        // Move construct a test object.
    {
    }

    // MANIPULATORS
    NonTrivialMovableAssignableDeletedSwapClass& operator=(
                           const NonTrivialMovableAssignableDeletedSwapClass&);
        // Assign a test object.

    // FRIENDS
    friend void swap(NonTrivialMovableAssignableDeletedSwapClass&,
                     NonTrivialMovableAssignableDeletedSwapClass&) = delete;
};
#endif

struct TrivialClass {
    // Empty 'struct'
};

union TrivialUnion {
    // Empty 'union'
};

}  // close unnamed namespace


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

struct MyType1 {
    // trivial so nothrow swappable
};

struct MyType2 {
    // private assignment, so not nothrow swappable
  private:
    // NOT IMPLEMENTED
    MyType2& operator=(const MyType2&);
        // Assignment operator made private to prevent swappability.
};

}  // close unnamed namespace

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
      case 4: {
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                              "=============\n");

#if defined(BSLS_COMPILERFEATURES_CPLUSPLUS) &&                               \
    BSLS_COMPILERFEATURES_CPLUSPLUS > 199711L
            // For C++03 the trait is not implemented so we do not perform the
            // test at all.

        ///Usage
        ///-----
        // In this section we show intended use of this component.
        //
        ///Example 1: Verify Class Types
        ///- - - - - - - - - - - - - - -
        // Suppose that we want to assert whether a particular type is nothrow
        // swappable.
        //
        // First, we create two 'struct's -- one nothrow swappable and one not.
        //..
        //  struct MyType1  // trivial so nothrow swappable
        //  {
        //  };
        //  struct MyType2  // private assignement, so not nothrow swappable
        //  {
        //  private:
        //      MyType2& operator=(const MyType2&);
        //  };
        //..
        // Now, we instantiate the 'bsl::is_nothrow_swappable' template for
        // each of the 'struct's and assert the 'value' static data member of
        // each instantiation:
        //..
#if defined(BSLMF_MSVC_ISNOTHROWSWAPPABLE_ALWAYS_FALSE)
        //For MSVC we have an implementation that deliberately always returns
        // false for C++14.
          ASSERT(false == bsl::is_nothrow_swappable<MyType1>::value);
          ASSERT(false == bsl::is_nothrow_swappable<MyType2>::value);
#else
          ASSERT(true  == bsl::is_nothrow_swappable<MyType1>::value);
          ASSERT(false == bsl::is_nothrow_swappable<MyType2>::value);
#endif
        //..
        // Note that if the current compiler supports the variable templates
        // C++14 feature then we can re-write the snippet of code above using
        // the 'bsl::is_nothrow_swappable_v' variable as follows:
        //..
        #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

#if defined(BSLMF_MSVC_ISNOTHROWSWAPPABLE_ALWAYS_FALSE)
        //For MSVC we have an implementation that deliberately always returns
        // false for C++14.
          ASSERT(false == bsl::is_nothrow_swappable_v<MyType1>);
          ASSERT(false == bsl::is_nothrow_swappable_v<MyType2>);
#else
          ASSERT(true  == bsl::is_nothrow_swappable_v<MyType1>);
          ASSERT(false == bsl::is_nothrow_swappable_v<MyType2>);
#endif

        #endif

        //..
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES
        //  It is not possible to determine whether an incomplete class type is
        //  nothrow swappable, or a reference to an incomplete type, as we may
        //  produce different results once the type is complete, leading to
        //  difficult to diagnose ODR violations.
        //
        // Concerns:
        //:  1 Instantiating the 'bsl::is_nothrow_swappable' is clearly
        //:    diagnosed with an error that cannot be circumvented by template
        //:    tricks exploiting SFINAE.
        //:
        //:  2 Pointers to incomplete types are perfectly valid types for which
        //:    the trait returns 'true'.
        //
        // Plan:
        //:  1 Under a pre-processor check requiring manual enablement, try to
        //:    instantiate 'bsl::is_nothrow_swappable' for incomplete class
        //:    types, and references thereof.
        //:
        //:  2 Verify that pointers to incomplete class types return 'true' for
        //:    this trait.
        //
        // Testing:
        //   QoI: diagnose incomplete types
        // --------------------------------------------------------------------

        if (verbose)
               printf("\nTESTING COMPILER DIAGNOSTICS AND INCOMPLETE TYPES"
                      "\n=================================================\n");

        // C-1
#if defined(BSLMF_ISNOTHROWSWAPPABLE_SHOW_ERROR_FOR_INCOMPLETE_CLASS)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT( bsl::is_nothrow_swappable<Incomplete>::value);
        ASSERT( bsl::is_nothrow_swappable<Incomplete&>::value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT( bsl::is_nothrow_swappable<Incomplete&&>::value);
#endif
#endif

#if defined(BSLMF_ISNOTHROWSWAPPABLE_SHOW_ERROR_FOR_INCOMPLETE_UNION)
        // The following ASSERT should produce a (non-SFINAEable) compile-time
        // error, and so cannot be enabled for regular automated test runs.
        // Define the test configuration macro above to enable a manual test
        // build to validate the error message.

        ASSERT( bsl::is_nothrow_swappable<Uncomplete>::value);
        ASSERT( bsl::is_nothrow_swappable<Uncomplete&>::value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT( bsl::is_nothrow_swappable<Uncomplete&&>::value);
#endif
#endif

        // C-2
        ASSERT_IS_NOTHROW_SWAPPABLE(Incomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(const Incomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(volatile Incomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(const volatile Incomplete *, true);

        ASSERT_IS_NOTHROW_SWAPPABLE(Uncomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(const Uncomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(volatile Uncomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(const volatile Uncomplete *, true);

        ASSERT_IS_NOTHROW_SWAPPABLE(Incomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(Incomplete * const, false);
        ASSERT_IS_NOTHROW_SWAPPABLE(Incomplete * volatile, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(Incomplete * const volatile, false);

        ASSERT_IS_NOTHROW_SWAPPABLE(Uncomplete *, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(Uncomplete * const, false);
        ASSERT_IS_NOTHROW_SWAPPABLE(Uncomplete * volatile, true);
        ASSERT_IS_NOTHROW_SWAPPABLE(Uncomplete * const volatile, false);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC SUPPORT FOR CLASS TYPES
        //   Ensure the 'bsl::is_nothrow_swappable' metafunction returns
        //   'false' for non-nothrow-swappable classes, and 'true' otherwise.
        //
        // Concerns:
        //:  1 The metafunction returns 'true' for non-trivial noexcept
        //:    swappable class types, and 'false' for cv-qualified non-trivial
        //:    class types.
        //:
        //:  2 The metafunction returns 'false' for class types that are not
        //:    nothrow swappable.
        //
        // Plan:
        //:  1 Verify that non-trivial class types that are regular object
        //:    types return 'true' for this trait, and that cv-qualified
        //:    versions thereof return false.
        //:
        //:  2 Verify that non-nothrow-swappable class types return 'false'
        //:    for this trait.
        //
        // Testing:
        //   CONCERN: basic support for class types
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC SUPPORT FOR CLASS TYPES"
                            "\n=====================================\n");


        // C-1
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialClass,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialUnion,
                            false,
                            false);

        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialClassWithFreeSwap,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialUnionWithFreeSwap,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialClassWithFreeNoexceptSwap,
                            true,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialUnionWithFreeNoexceptSwap,
                            true,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            TrivialClass,
                            true,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            TrivialUnion,
                            true,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialMovableAssignableClass,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialMovableNoexceptAssignableClass,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialNoexceptMovableAssignableClass,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialNoexceptMovableNoexceptAssignableClass,
                            true,
                            false);

        // C-2
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialPrivateMovePrivateAssignment,
                            false,
                            false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS)
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialDeletedMoveDeletedAssignmentClass,
                            false,
                            false);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(
                            NonTrivialMovableAssignableDeletedSwapClass,
                            false,
                            false);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_nothrow_swappable::value'
        //   Ensure the 'bsl::is_nothrow_swappable' metafunction
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
        //: 10 That 'is_nothrow_swappable<T>::value' has the same
        //:    value as 'is_nothrow_swappable_v<T>' for a variety of
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
        //   bsl::is_nothrow_swappable::value
        //   bsl::is_nothrow_swappable_v
        // --------------------------------------------------------------------

        if (verbose)
             printf("\nTESTING 'bsl::is_nothrow_swappable::value'"
                    "\n==========================================\n");

        // C-1
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(bool, true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(char, true, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(char8_t, true, true);
#endif
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(int,  true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(long double,    true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(bsl::nullptr_t, true, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(char16_t,       true, true);
#endif

        // C-2
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(EnumTestType,   true, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(EnumClassType,  true, true);
#endif

        // C-3
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(DataMemberPtrTestType,
                                                         true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(MethodPtrTestType,
                                                         true, true);


        // C-4 : 'void' is not an object type, and although it can be
        // cv-qualified, there are no references to 'void' so we must drop to
        // the most primitive test macro:
        ASSERT_IS_NOTHROW_SWAPPABLE(void,                false);
        ASSERT_IS_NOTHROW_SWAPPABLE(const void,          false);
        ASSERT_IS_NOTHROW_SWAPPABLE(volatile void,       false);
        ASSERT_IS_NOTHROW_SWAPPABLE(const volatile void, false);

        // C-6 : Pointers to void are perfectly good object types:
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(void*,          true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(const void*,    true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(volatile void*, true, true);
        ASSERT_IS_NOTHROW_SWAPPABLE_OBJECT_TYPE(const volatile void*,
                                                                true, true);

        // C-5 : Function types are not object types, nor cv-qualifiable.
        ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(void(),               false);
        ASSERT_IS_NOTHROW_SWAPPABLE_TYPE(int(float,double...), false);
        ASSERT_IS_NOTHROW_SWAPPABLE(void() const,              false);
        ASSERT_IS_NOTHROW_SWAPPABLE(int(float,double...) const,false);
      } break;
      default: {
          fprintf(stderr, "WARNING: CASE '%d' NOT FOUND.\n", test);
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
