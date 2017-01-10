// bslmf_iscopyconstructible.t.cpp                                    -*-C++-*-
#include <bslmf_iscopyconstructible.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addvolatile.h>
#include <bslmf_ispointer.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bsl::is_copy_constructible', that determines whether a template parameter
// type is copy constructible.  By default on C++03 platforms, the
// meta-function must be extended to support user-defined non-copyable types
// through either template specialization or use of the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by the meta-function by testing the meta-function with each of
// the supported type categories.  We also need to verify that the
// meta-function can be correctly extended to support other types through
// either of the two supported mechanisms.  Finally, we need to test correct
// support for cv-qualified and array types, where the underlying type may be
// have a copy constructor.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_copy_constructible::value
//
// ----------------------------------------------------------------------------
// [ 2] EXTENDING 'bsl::is_copy_constructible'
// [  ] USAGE EXAMPLE

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
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below will test the 'bsl::is_copy_constructible' trait
// with a set of variations on a type.  There are several layers of macros, as
// object types support the full range of variation, but function types cannot
// form an array, nor be cv-qualified.  Similarly, 'void' may be cv-qualified
// but still cannot form an array.  As macros are strictly text-substitution
// we must use the appropriate 'add_decoration' traits to transform types in a
// manner that is guaranteed to be syntactically valid.  Note that these are
// not type-dependent contexts, so there is no need to use 'typename' when
// fetching the result from any of the queried traits.

#define ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE(TYPE, RESULT)                       \
    ASSERT( bsl::is_copy_constructible<TYPE>::value == RESULT);               \
    ASSERT( bsl::is_copy_constructible<                                       \
                               bsl::add_lvalue_reference<TYPE>::type>::value);

#define ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                    \
{   const bool IS_PRIMITIVE = bsl::is_fundamental<TYPE>::value                \
                           || bsl::is_enum<TYPE>::value                       \
                           || bsl::is_pointer<TYPE>::value                    \
                           || bsl::is_member_pointer<TYPE>::value;            \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE(TYPE, RESULT);                          \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE(bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE(bsl::add_volatile<TYPE>::type,          \
                                                               IS_PRIMITIVE); \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE(bsl::add_cv<TYPE>::type, IS_PRIMITIVE); \
}

// Two additional macros will allow testing on old MSVC compilers when 'TYPE'
// is an array of unknown bound.

#define ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT)                \
    ASSERT( bsl::is_copy_constructible<TYPE>::value == RESULT);

#define ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE, RESULT)             \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT);                   \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE_NO_REF(                                 \
                                         bsl::add_const<TYPE>::type, RESULT); \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE_NO_REF(                                 \
                                      bsl::add_volatile<TYPE>::type, false);  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_TYPE_NO_REF(bsl::add_cv<TYPE>::type, false);


#if defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems

# define ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)               \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                        \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(bsl::add_pointer<TYPE>::type, true)  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[128], false)                    \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], false)

#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
// Old microsoft compilers compilers do not support references to arrays of
// unknown bound.

# define ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)               \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                        \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(bsl::add_pointer<TYPE>::type, true)  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[128], false)                    \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], false)                  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[], false)                \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[][8], false)

#else
# define ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)               \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                        \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(bsl::add_pointer<TYPE>::type, true)  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[128], false)                    \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], false)                  \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[], false)                       \
    ASSERT_IS_COPY_CONSTRUCTIBLE_CV_TYPE(TYPE[][8], false)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class UserDefinedNcTestType {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

  private:
    // NOT IMPLEMENTED
    UserDefinedNcTestType(const UserDefinedNcTestType&);  // private
  public:
    UserDefinedNcTestType() {}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

class UserDefinedNcTestType2 {
    // This user-defined type, which is marked to have a 'nothrow' move
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

  private:
    // NOT IMPLEMENTED
    UserDefinedNcTestType2(const UserDefinedNcTestType2&);  // private
  public:
    // BSLMF_NESTED_TRAIT_DECLARATION_IF(UserDefinedNcTestType2,
    //                                   bsl::is_copy_constructible,
    //                                   false);

    UserDefinedNcTestType2() {}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UserDefinedCopyableTestType {
    // This user-defined type, which is not marked to be 'nothrow' move
    // constructible, is used for testing.

    UserDefinedCopyableTestType() {}
    UserDefinedCopyableTestType(const UserDefinedCopyableTestType& original)
    {
        (void) original;
    }
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum class EnumClassType {
    // This 'enum' type is used for testing.
};
#endif

typedef int (UserDefinedCopyableTestType::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

}  // close unnamed namespace


namespace bsl {

#if !defined(BSLS_ISCOPYCONSTRUCTIBLE_USE_NATIVE_TRAIT)
template <>
struct is_copy_constructible<UserDefinedNcTestType> : false_type {};
template <>
struct is_copy_constructible<UserDefinedNcTestType2> : false_type {};
#endif

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
      case 2: {
        // --------------------------------------------------------------------
        // EXTENDING 'bsl::is_copy_constructible'
        //   Ensure the 'bsl::is_copy_constructible' meta-function
        //   returns the correct value for types explicitly specified to have
        //   a 'nothrow' move constructor.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for normal user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bsl::is_copy_constructible' on that
        //    type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 For cv-qualified types, the meta-function returns 'true' if the
        //:   corresponding cv-unqualified type is 'nothrow' move constructible
        //    and 'false' otherwise.
        //:
        //: 5 For array types, the meta-function returns 'true' if the array
        //:   element is 'nothrow' move constructible, and 'false' otherwise.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C4,5)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-3, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   EXTENDING 'bsl::is_copy_constructible'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nEXTENDING 'bsl::is_copy_constructible'"
                   "\n======================================\n");

        // C-1
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(UserDefinedCopyableTestType,
                                                 true);

        // C-2
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(UserDefinedNcTestType, false);

        // C-3
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(UserDefinedNcTestType2,
                                                                        false);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_copy_constructible::value'
        //   Ensure the 'bsl::is_copy_constructible' meta-function
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The meta-function returns 'true' for fundamental types.
        //:
        //:  2 The meta-function returns 'true' for enumerated types.
        //:
        //:  3 The meta-function returns 'true' for pointer to member types.
        //:
        //:  4 The meta-function returns 'false' for cv-qualified 'void' types.
        //:
        //:  5 The meta-function returns 'false' for function types.
        //
        //:  6 The meta-function returns 'true' for pointer types.
        //:
        //:  7 The meta-function returns 'false' for reference types.
        //:
        //:  8 The meta-function returns the same result for array types as it
        //:    would for the array's element type.
        //:
        //:  9 The meta-function returns the same result for cv-qualified
        //:    types that it would return 'true' for the corresponding
        //:    cv-unqualified type.
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
        //   bsl::is_copy_constructible::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_copy_constructible::value'"
                   "\n===================================\n");

        // C-1
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(int,  true);
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(char, true);
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(long double, true);

        // C-2
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(EnumTestType, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(EnumClassType, true);
#endif

        // C-3
        ASSERT_IS_COPY_CONSTRUCTIBLE_OBJECT_TYPE(MethodPtrTestType, true);

        // C-4 : 'void' is not an object type, but can be cv-qualified.
        ASSERT(!bsl::is_copy_constructible<               void  >::value);
        ASSERT( bsl::is_copy_constructible<               void *>::value);
        ASSERT(!bsl::is_copy_constructible<const          void  >::value);
        ASSERT( bsl::is_copy_constructible<const          void *>::value);
        ASSERT(!bsl::is_copy_constructible<      volatile void  >::value);
        ASSERT( bsl::is_copy_constructible<      volatile void *>::value);
        ASSERT(!bsl::is_copy_constructible<const volatile void  >::value);
        ASSERT( bsl::is_copy_constructible<const volatile void *>::value);

        // C-5 : Function types are not object types, nor cv-qualifiable.
        // Note that this particular test stresses compilers handling of
        // function types, and function reference types, in the template type
        // system.  We incrementally disable tests for compilers known to have
        // bugs parsing function type parameters that we cannot easily work
        // around.  Failing to test such cases is not a concern, as users
        // cannot make such a request either.
        ASSERT( bsl::is_copy_constructible<void(*)()>::value);
        ASSERT( bsl::is_copy_constructible<int(*)(float, double...)>::value);
#if !defined(BSLS_PLATFORM_CMP_SUN) // last tested for v12.3
        ASSERT(!bsl::is_copy_constructible<void()>::value);
        ASSERT(!bsl::is_copy_constructible<int(float, double...)>::value);
#if !defined(BSLS_PLATFORM_CMP_IBM) // last tested for v12.1
        ASSERT(bsl::is_copy_constructible<void(&)()>::value);
        ASSERT(bsl::is_copy_constructible<int(&)(float, double...)>::value);
#endif
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
// Copyright 2016 Bloomberg Finance L.P.
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
