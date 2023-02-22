// bslmf_isfundamental.t.cpp                                          -*-C++-*-

#include <bslmf_isfundamental.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_fundamental'
// and 'bslmf::IsFundamental' and a template variable 'bsl::is_fundamental_v',
// that determine whether a template parameter type is a fundamental type.
// Thus, we need to ensure that the values returned by these meta-functions are
// correct for each possible category of types.  Since the two meta-functions
// are almost functionally equivalent except on reference to fundamental types,
// we will use the same set of types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsFundamental::VALUE
// [ 1] bsl::is_fundamental::value
// [ 1] bsl::is_fundamental_v
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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

// Each of the macros below will test a 'METAFUNC' trait type with a set of
// variations on a 'TYPE'.  There are several layers of macros, as object types
// support the full range of variation, but function types cannot form an
// array, nor be cv-qualified.  Similarly, 'void' may be cv-qualified but still
// cannot form an array.  As macros are strictly text-substitution we must use
// the appropriate 'add_decoration' traits to transform types in a manner that
// is guaranteed to be syntactically valid.  Note that these are not
// type-dependent contexts, so there is no need to use 'typename' when fetching
// the result from any of the queried traits.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER) &&                   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define LEGACY_ASSERT_REF(META_FUNC, TYPE, result)                           \
    ASSERT(result == META_FUNC<                          TYPE       >::VALUE);\
    ASSERT(result == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::VALUE);\
    ASSERT(false  == META_FUNC<bsl::add_rvalue_reference<TYPE>::type>::VALUE);
#else
# define LEGACY_ASSERT_REF(META_FUNC, TYPE, result)                           \
    ASSERT(result == META_FUNC<                          TYPE       >::VALUE);\
    ASSERT(result == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::VALUE);
#endif
    // Lvalue-references are deemed to be fundamental, for purposes of a legacy
    // trait, as long as they refer to a fundamental types; this macro tests
    // whether a given 'TYPE' is fundamental according to a legacy METAFUNC
    // trait, and whether any kind of reference to that TYPE is fundamental.

# define LEGACY_ASSERT_CVQ(META_FUNC, TYPE, result)                          \
         LEGACY_ASSERT_REF(META_FUNC,                   TYPE,        result) \
         LEGACY_ASSERT_REF(META_FUNC, bsl::add_const<   TYPE>::type, result) \
         LEGACY_ASSERT_REF(META_FUNC, bsl::add_volatile<TYPE>::type, result) \
         LEGACY_ASSERT_REF(META_FUNC, bsl::add_cv<      TYPE>::type, result)
    // Test all cv-qualified combinations on a type, and references to those
    // same cv-qualified types.

# define LEGACY_ASSERT_CVQP(META_FUNC,                  TYPE,        result) \
         LEGACY_ASSERT_CVQ (META_FUNC,                  TYPE,        result) \
         LEGACY_ASSERT_CVQ (META_FUNC, bsl::add_pointer<TYPE>::type, false)
    // Test whether a type is fundamental, and confirm that pointers to such a
    // type are never fundamental.

# define LEGACY_ASSERT_CVQ_NO_REF(META_FUNC, TYPE, result)                 \
    ASSERT(result == META_FUNC<                  TYPE       >::VALUE);     \
    ASSERT(result == META_FUNC<bsl::add_const<   TYPE>::type>::VALUE);     \
    ASSERT(result == META_FUNC<bsl::add_volatile<TYPE>::type>::VALUE);     \
    ASSERT(result == META_FUNC<bsl::add_cv<      TYPE>::type>::VALUE);
    // Test all cv-qualified combinations on a type, but not references to that
    // type.

# define LEGACY_ASSERT_CVQP_NO_REF(META_FUNC,           TYPE,        result) \
         LEGACY_ASSERT_CVQ_NO_REF (META_FUNC,           TYPE,        result) \
         LEGACY_ASSERT_CVQ (META_FUNC, bsl::add_pointer<TYPE>::type, false)
    // Test whether a type is fundamental, and confirm that pointers to such a
    // type are never fundamental.


#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#define ASSERT_V_SAME(TYPE)                                                   \
    ASSERT(bsl::is_fundamental<TYPE>::value == bsl::is_fundamental_v<TYPE>)
    // Test whether 'bsl::is_fundamental_v<TYPE>' has the same value as
    // 'bsl::is_fundamental<TYPE>::value'.
#else
#define ASSERT_V_SAME(TYPE)
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER) &&                   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TYPE_ASSERT_REF(META_FUNC, TYPE, result)                             \
    ASSERT(result == META_FUNC<                         TYPE       >::value); \
    ASSERT(false == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::value); \
    ASSERT(false == META_FUNC<bsl::add_rvalue_reference<TYPE>::type>::value); \
    ASSERT_V_SAME(                                      TYPE               ); \
    ASSERT_V_SAME(            bsl::add_lvalue_reference<TYPE>::type        ); \
    ASSERT_V_SAME(            bsl::add_rvalue_reference<TYPE>::type        );
#else
# define TYPE_ASSERT_REF(META_FUNC, TYPE, result)                             \
    ASSERT(result == META_FUNC<                         TYPE       >::value); \
    ASSERT(false == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::value);
#endif
    // References are never fundamental types, but must safely be parsed by the
    // metafunction under test, and give the expected ('false') result.

# define TYPE_ASSERT_CVQ(META_FUNC, TYPE, result)                          \
         TYPE_ASSERT_REF(META_FUNC,                   TYPE,        result) \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_const<   TYPE>::type, result) \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_volatile<TYPE>::type, result) \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_cv<      TYPE>::type, result)
    // Test all cv-qualified combinations on a type, and references to those
    // same cv-qualified types.

# define TYPE_ASSERT_CVQP(META_FUNC,                  TYPE,        result) \
         TYPE_ASSERT_CVQ (META_FUNC,                  TYPE,        result) \
         TYPE_ASSERT_CVQ (META_FUNC, bsl::add_pointer<TYPE>::type, false)
    // Test whether a type is fundamental, and confirm that pointers to such a
    // type is never fundamental.

# define TYPE_ASSERT_CVQ_NO_REF(META_FUNC, TYPE, result)                   \
    ASSERT(result == META_FUNC<                  TYPE       >::value);     \
    ASSERT(result == META_FUNC<bsl::add_const<   TYPE>::type>::value);     \
    ASSERT(result == META_FUNC<bsl::add_volatile<TYPE>::type>::value);     \
    ASSERT(result == META_FUNC<bsl::add_cv<      TYPE>::type>::value);     \
    ASSERT_V_SAME(                               TYPE               );     \
    ASSERT_V_SAME(             bsl::add_const<   TYPE>::type        );     \
    ASSERT_V_SAME(             bsl::add_volatile<TYPE>::type        );     \
    ASSERT_V_SAME(             bsl::add_cv<      TYPE>::type        );
    // Test all cv-qualified combinations on a type, but not references to that
    // type.

# define TYPE_ASSERT_CVQP_NO_REF(META_FUNC,           TYPE,        result) \
         TYPE_ASSERT_CVQ_NO_REF (META_FUNC,           TYPE,        result) \
         TYPE_ASSERT_CVQ (META_FUNC, bsl::add_pointer<TYPE>::type, false)
    // Test whether a type is fundamental, and confirm that pointers to such a
    // type is never fundamental, but do not test references to such a type.


#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_ISFUNDAMENTAL_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND 1
        // The IBM xlC compiler does not handle arrays of unknown bounds as
        // template type parameters.  Older Microsoft compilers have problems
        // with references to arrays of unknown bound that fall out of the
        // template metaprograms used to implement this trait.
#endif

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consequently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_fundamental'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_fundamental'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_fundamental'.
};

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.

typedef int (Incomplete::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_fundamental'.

typedef int Incomplete::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_fundamental'.

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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Fundamental Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are fundamental types.
//
// Now, we instantiate the 'bsl::is_fundamental' template for several
// non-fundamental and fundamental types, and assert the 'value' static data
// member of each instantiation:
//..
    ASSERT(true  == bsl::is_fundamental<int>::value);
    ASSERT(false == bsl::is_fundamental<int&>::value);
    ASSERT(true  == bsl::is_fundamental<long long  >::value);
    ASSERT(false == bsl::is_fundamental<long long *>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_function_v' variable as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(true  == bsl::is_fundamental_v<int>);
    ASSERT(false == bsl::is_fundamental_v<int&>);
    ASSERT(true  == bsl::is_fundamental_v<long long  >);
    ASSERT(false == bsl::is_fundamental_v<long long *>);
#endif
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsFundamental::VALUE'
        //   Ensure that the static data member 'VALUE' of
        //   'bslmf::IsFundamental' instantiations having various (template
        //   parameter) 'TYPE's has the correct value.
        //
        // Concerns:
        //: 1 'IsFundamental::VALUE' is 1 when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type or reference to (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'IsFundamental::VALUE' is 0 when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'IsFundamental::VALUE' is 0 when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) type.
        //:
        //: 4 'IsFundamental::VALUE' is 0 when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //:
        //: 5 'is_fundamental::value' is '1' when 'TYPE' is a (possibly
        //:   cv-qualified) 'bsl::nullptr_t' type if the 'nullptr' keyword is
        //:   is supported, and '0' otherwise.
        //
        // Plan:
        //   Verify that 'bslmf::IsFundamental::VALUE' has the correct value
        //   for each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bslmf::IsFundamental::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("'bslmf::IsFundamental::VALUE'\n"
                            "=============================\n");
        // C-1
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, char,                   1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, signed char,            1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, unsigned char,          1);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, char8_t,                1);
#endif
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, wchar_t,                1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, short int,              1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, unsigned short int,     1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int,                    1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, unsigned int,           1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, long int,               1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, unsigned long int,      1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, float,                  1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, double,                 1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, long double,            1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, long long int,          1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, unsigned long long int, 1);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_LEGACYS)
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, char16_t,               1);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, char32_t,               1);
#endif

        LEGACY_ASSERT_CVQP_NO_REF(bslmf::IsFundamental, void,            1);

        // C-2
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, EnumTestType,           0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, StructTestType,         0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, UnionTestType,          0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, Incomplete,             0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, BaseClassTestType,      0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, DerivedClassTestType,   0);

        // C-3
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int StructTestType::*,  0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, MethodPtrTestType,      0);

        // C-4
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int  (),                0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, void (char...),         0);

        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int[5],                 0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int[5][42],             0);
#if !defined(BSLMF_ISFUNDAMENTAL_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND)
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int[ ],                 0);
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, int[ ][42],             0);
#endif

        // C -5
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, bsl::nullptr_t,         1);
#else
        LEGACY_ASSERT_CVQP(bslmf::IsFundamental, bsl::nullptr_t,         0);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_fundamental::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_fundamental' instantiations having various (template
        //   parameter) 'TYPE's has the correct value.
        //
        // Concerns:
        //: 1 'is_fundamental::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type, and 'false' when 'TYPE' is a
        //:   reference to (possibly cv-qualified) primitive type.
        //:
        //: 2 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) type.
        //:
        //: 4 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //:
        //: 5 'is_fundamental::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'bsl::nullptr_t' type if the 'nullptr' keyword is
        //:   is supported, and 'false' otherwise.
        //:
        //: 6  That 'is_fundamental<T>::value' has the same value as
        //:    'is_fudamental_v<T>' for a variety of template parameter types.
        //
        // Plan:
        //   Verify that 'bsl::is_fundamental::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_fundamental::value
        //   bsl::is_fundamental_v
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_fundamental::value'\n"
                            "============================\n");

        // C-1
        TYPE_ASSERT_CVQP(bsl::is_fundamental, char,                   true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, signed char,            true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, unsigned char,          true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TYPE_ASSERT_CVQP(bsl::is_fundamental, char8_t,                true);
#endif
        TYPE_ASSERT_CVQP(bsl::is_fundamental, wchar_t,                true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, short int,              true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, unsigned short int,     true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int,                    true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, unsigned int,           true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, long int,               true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, unsigned long int,      true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, float,                  true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, double,                 true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, long double,            true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, long long int,          true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, unsigned long long int, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TYPE_ASSERT_CVQP(bsl::is_fundamental, char16_t,               true);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, char32_t,               true);
#endif

        TYPE_ASSERT_CVQP_NO_REF(bsl::is_fundamental, void,            true);

        // C-2
        TYPE_ASSERT_CVQP(bsl::is_fundamental, EnumTestType,           false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, StructTestType,         false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, UnionTestType,          false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, Incomplete,             false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, BaseClassTestType,      false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, DerivedClassTestType,   false);

        // C-3
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int StructTestType::*,  false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, MethodPtrTestType,      false);

        // C-4
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int  (),                false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, void (char...),         false);

        TYPE_ASSERT_CVQP(bsl::is_fundamental, int[5],                 false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int[5][42],             false);
#if !defined(BSLMF_ISFUNDAMENTAL_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND)
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int[ ],                 false);
        TYPE_ASSERT_CVQP(bsl::is_fundamental, int[ ][42],             false);
#endif

        // C -5
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
        TYPE_ASSERT_CVQP(bsl::is_fundamental, bsl::nullptr_t,          true);
#else
        TYPE_ASSERT_CVQP(bsl::is_fundamental, bsl::nullptr_t,         false);
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
