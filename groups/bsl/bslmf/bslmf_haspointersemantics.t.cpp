// bslmf_haspointersemantics.t.cpp                                    -*-C++-*-
#include <bslmf_haspointersemantics.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a metafunction,
// 'bslmf::HasPointerSemantics', which determines whether a template parameter
// type is intended to be dereferenced like a pointer.  By default, the
// metafunction supports only naitve pointers, including non-derefencable
// pointer types such as 'void *' and function pointers,  and can be extended
// to support other types through either template specialization or use of the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by checking the metafunction correctly returns 'false_type' for
// each of the supported type categories, and returns 'true_type' for native
// pointers.  We also need to verify that the metafunction can be correctly
// extended to support user-defined types through either of the two supported
// mechanisms.  Finally, we need to test correct support for cv-qualified types
// and array types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslmf::HasPointerSemantics::value
// ----------------------------------------------------------------------------
// [  ] USAGE EXAMPLE
// [ 2] EXTENDING 'bslmf::HasPointerSemantics'

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
//                  COMPILER DEFECT MACROS TO GUIDE TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
# define BSLMF_HASPOINTERSEMANTICS_ABOMINABLE_FUNCTION_MATCH_CONST 1
// The Solaris CC compiler matches 'const' qualified abominable functions as
// 'const'-qualified template parameters, but does not strip the 'const'
// qualifier when passing that template parameter onto the next instantiation.
// Therefore, 'HasPointerSemantics<void() const>' requests infinite
// template recursion.  We opt to not try a workaround in the header for this
// platform, where we would delegate to the same implementation as the primary
// template, as that would leave an awkward difference in behavior for 'const'
// qualified class types between using a nested trait and directly specializing
// the trait.  Likeiwise, we choose not to add a compiler-specific workaround
// using 'bsl::is_function' to conditionally produce 'false_type' rather than
// delegate to the primary template, as the additional template instantiation
// overhead is not worth the cost of supporting such an awkaward corner case.
// Abominable function types are a sufficiently unlikely to occur in production
// code that the risk from simply silencing this test case (on just this broken
// platform) is negligible.
#endif

#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 0x1900)   \
 || defined(BSLMF_HASPOINTERSEMANTICS_ABOMINABLE_FUNCTION_MATCH_CONST)
# define BSLMF_HASPOINTERSEMANTICS_NO_ABOMINABLE_FUNCTIONS  1
// Older MSVC compilers do not parse abominable function types, so it does not
// matter whether trait would support them or not, we can simply disable such
// tests on this platform.
#endif

//=============================================================================
//                  MACROS TO CONFIGURE TESTING
//-----------------------------------------------------------------------------

//#define BSLMF_HASPOINTERSEMANTICS_TEST_INCOMPLETE_TYPES_FAIL    1
// Define this macro to enable test coverage of incomplete class types, which
// should produce a compile-error.  Testing with this macro must be enabled
// outside the regular nightly regression cycle, as by its existence, it would
// cause the test driver to fail.

//=============================================================================
//                  COMPONENT-SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below will test the 'bslmf::HasPointerSemantics'
// trait with a set of variations on a type.  There are several layers of
// macros, as object types support the full range of variation, but function
// types cannot form an array, nor be cv-qualified.  Similarly, 'void' may be
// cv-qualified but still cannot form an array.  As macros are strictly
// text-substitution we must use the appropriate 'add_decoration' traits to
// transform types in a manner that is guaranteed to be syntactically valid.
// Note that these are not type-dependent contexts, so there is no need to use
// 'typename' when fetching the result from any of the queried traits.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define ASSERT_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT)                       \
    ASSERT( bslmf::HasPointerSemantics<TYPE>::value == RESULT);               \
    ASSERT( bslmf::HasPointerSemantics<bsl::add_pointer<TYPE>::type>::value); \
    ASSERT(!bslmf::HasPointerSemantics<                                       \
                              bsl::add_lvalue_reference<TYPE>::type>::value); \
    ASSERT(!bslmf::HasPointerSemantics<                                       \
                              bsl::add_rvalue_reference<TYPE>::type>::value)
#else
#define ASSERT_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT)                       \
    ASSERT( bslmf::HasPointerSemantics<TYPE>::value == RESULT);               \
    ASSERT( bslmf::HasPointerSemantics<bsl::add_pointer<TYPE>::type>::value); \
    ASSERT(!bslmf::HasPointerSemantics<                                       \
                                 bsl::add_lvalue_reference<TYPE>::type>::value)
#endif
    // Pointers always has pointer semantincs, and references are never
    // pointer-like, regardless of the type they refer to.

#define ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT)                    \
    ASSERT_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT);                          \
    ASSERT_TYPE_HAS_POINTER_SEMANTICS(bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_TYPE_HAS_POINTER_SEMANTICS(bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_TYPE_HAS_POINTER_SEMANTICS(bsl::add_cv<TYPE>::type, RESULT)
    // Confirm that all cv-qualified variations on a type produce the same
    // result as for the specified 'TYPE' itself.


#define ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT)                \
    ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE, RESULT);                       \
    ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE[128], false);                   \
    ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE[12][8], false) ;                \
    ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE[], false);                      \
    ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(TYPE[][8], false)
    // For an object type, confirm that specified 'TYPE' has the expected
    // result for the tested trait, and arrays of that type produce a matching
    // result.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This 'enum' type is used for testing.
};

class Incomplete;
union Uncomplete;

struct NotAPointer {};

typedef int Incomplete::* MemberDataTestType;
    // This pointer to non-static data member type is used for testing.

typedef int (Incomplete::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

struct PointerLikeNested {
    BSLMF_NESTED_TRAIT_DECLARATION(PointerLikeNested,
                                   bslmf::HasPointerSemantics);
};

struct PointerLikeSpecialized {};
}  // close unnamed namespace

namespace BloombergLP {
namespace bslmf {

template <>
struct HasPointerSemantics<PointerLikeSpecialized> : bsl::true_type {};
}  // close package namespace
}  // close enterprise namespace


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

    (void)         veryVerbose;  // eliminate unused variable warning
    (void)     veryVeryVerbose;  // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // EXTENDING 'bslmf::HasPointerSemantics'
        //   Ensure the 'bslmf::HasPointerSemantics' metafunction
        //   returns the correct value for types explicitly specialized to
        //   support this trait.  Note that the only user-defined types that
        //   can be customized are class types and union types.
        //
        // Concerns:
        //: 1 The metafunction returns 'false' for plain user-defined types,
        //:   which may be classes or unions.
        //:
        //: 2 The metafunction returns 'true' for a user-defined type, if a
        //:   specialization for 'bslmf::HasPointerSemantics' on that
        //:   type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The metafunction returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 The metafunction returns the same result for a cv-qualified type
        //:   as for the corresponding cv-unqualified type.
        //:
        //: 5 The metafunction returns the same result for an array type as for
        //:   the array's element type.
        //:
        //: 6 The metafunction gives a hard error when instantiated with an
        //:   incomplete class type, rather than risk differing results when
        //:   the type is completed.
        //
        // Plan:
        //:  1 Create a macro that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C-1..5)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //: 2 For user defined types with no customization, use the test macro
        //:   to confirm the metafunction returns 'false' in each case.
        //:
        //: 3 For user defined types utilizing each customization point in
        //:   turn, use the test macro to confirm the metafunction returns
        //:   'true' in each case.
        //:
        //:  4 Guarded by a configuration macro, which defaults to unchecked,
        //:    provide additional tests to ensure hard errors are diagnosed
        //:    for incomplete object types. (C-6)
        //
        // Testing:
        //   EXTENDING 'bslmf::HasPointerSemantics'
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nEXTENDING 'bslmf::HasPointerSemantics'"
                         "\n==============================================\n");

        // C-1
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(NotAPointer,false);

        // C-2
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(PointerLikeNested, true);

        // C-3
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(PointerLikeSpecialized, true);

        // C-6 Verify there are no surprises for incomplete types.

#if defined(BSLMF_HASPOINTERSEMANTICS_TEST_INCOMPLETE_TYPES_FAIL)
        // It is impossible to test for a nested trait in an incomplate type,
        // so the trait should fail to instantiate.
        ASSERT(!bslmf::HasPointerSemantics<               Incomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<const          Incomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<      volatile Incomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<const volatile Incomplete>::value);

        ASSERT(!bslmf::HasPointerSemantics<               Uncomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<const          Uncomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<      volatile Uncomplete>::value);
        ASSERT(!bslmf::HasPointerSemantics<const volatile Uncomplete>::value);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::HasPointerSemantics::value'
        //   Ensure the 'bslmf::HasPointerSemantics' metafunction
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The metafunction returns 'false' for fundamental object types.
        //:
        //:  2 The metafunction returns 'false' for cv-qualified 'void' types.
        //:
        //:  3 The metafunction returns 'false' for enumerated types.
        //:
        //:  4 The metafunction returns 'false' for pointer to member types.
        //:
        //:  5 The metafunction returns 'false' for function types.
        //:
        //:  6 The metafunction returns 'true' for pointer types.
        //:
        //:  7 The metafunction returns 'false' for reference types.
        //:
        //:  8 The metafunction returns 'false' for array types.
        //:
        //:  9 The metafunction returns the same result for a cv-qualified type
        //:    as for the corresponding cv-unqualified type.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all (legal) variants of a type:  (C-7..9; partial for 6)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and unknown bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-5, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types. (C-1..5)
        //:
        //:  3 To complete testing of pointers not covered by earlier tests,
        //:    provide additional tests for pointers to function types, and for
        //:    pointers to array types. (C-6)
        //
        // Testing:
        //   bslmf::HasPointerSemantics::value
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTESTING 'bslmf::HasPointerSemantics::value'"
                    "\n===================================================\n");

        // C-1 : Test all fundamental types to be sure there are no accidental
        // gaps in coverage.

        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(char, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(signed char, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(unsigned char, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(wchar_t, false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(char16_t, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(char32_t, false);
#endif

        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(short, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(unsigned short, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(int, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(unsigned int, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(long, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(unsigned long, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(long long, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(unsigned long long, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(long, false);

        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(bool, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(bsl::nullptr_t, false);

        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(float, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(double, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(long double, false);

        // C-2 : 'void' is not an object type, but can be cv-qualified.

        ASSERT_CV_TYPE_HAS_POINTER_SEMANTICS(void, false);

        // C-3
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(EnumTestType, false);

        // C-4
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(MemberDataTestType, false);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(MethodPtrTestType, false);

        // C-5 : Function types are neither object types nor cv-qualifiable, so
        // must use the simplest test macro.  Abominable function types support
        // neither references nor pointers, so must be tested directly.

        ASSERT_TYPE_HAS_POINTER_SEMANTICS(void(), false);
        ASSERT_TYPE_HAS_POINTER_SEMANTICS(bool(float, double...), false);
#if !defined(BSLMF_HASPOINTERSEMANTICS_NO_ABOMINABLE_FUNCTIONS)
        ASSERT(!bslmf::HasPointerSemantics<void() volatile>::value);
        ASSERT(!bslmf::HasPointerSemantics<void(...) const>::value);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        ASSERT_TYPE_HAS_POINTER_SEMANTICS(void() noexcept, false);
        ASSERT_TYPE_HAS_POINTER_SEMANTICS(bool(float, double...) noexcept,
                                          false);
        ASSERT(!bslmf::HasPointerSemantics<void() const noexcept>::value);
#endif

        // C-6 : Pointer types are mostly tested as object types in the macros
        // above, but we should add a few extra cases to recursively validate
        // them as object types in their own right.

        // Typedefs are needed to avoid strange parsings in the test macros.
        typedef int Array1D[42];
        typedef int Array2D[42][13];
        typedef int ArrayUB[];
        typedef int ArrayU2[][13];
        typedef void VoidFn();
        typedef bool MoreFn(float, double...);

        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(int *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(void *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(Incomplete *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(Array1D *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(Array2D *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(ArrayUB *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(ArrayU2 *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(VoidFn *, true);
        ASSERT_OBJECT_TYPE_HAS_POINTER_SEMANTICS(MoreFn *, true);
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
