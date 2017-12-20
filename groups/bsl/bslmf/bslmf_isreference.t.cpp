// bslmf_isreference.t.cpp                                            -*-C++-*-
#include <bslmf_isreference.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_reference', that
// determines whether a template parameter type is an (lvalue or rvalue)
// reference type.  Thus, we need to ensure that the value returned by this
// meta-function is correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_reference::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consquently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_CMP_GNU)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180) // cv-qualifiers meaningless on function types
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_reference'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_reference'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_reference'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_reference'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_reference'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_reference'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_reference'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_reference'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_reference'.

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, type, result)                \
    ASSERT(result == metaFunc<type>::member);                                 \
    ASSERT(result == metaFunc<type const>::member);                           \
    ASSERT(result == metaFunc<type volatile>::member);                        \
    ASSERT(result == metaFunc<type const volatile>::member);

#define TYPE_ASSERT_CVQ_PREFIX(metaFunc, member, type, result)                \
    ASSERT(result == metaFunc<type>::member);                                 \
    ASSERT(result == metaFunc<const type>::member);                           \
    ASSERT(result == metaFunc<volatile type>::member);                        \
    ASSERT(result == metaFunc<const volatile type>::member);

#define TYPE_ASSERT_CVQ_REF(metaFunc, member, type, result)                   \
    ASSERT(result == metaFunc<type&>::member);                                \
    ASSERT(result == metaFunc<type const&>::member);                          \
    ASSERT(result == metaFunc<type volatile&>::member);                       \
    ASSERT(result == metaFunc<type const volatile&>::member);

#define TYPE_ASSERT_CVQ_RREF(metaFunc, member, type, result)                  \
    ASSERT(result == metaFunc<type&&>::member);                               \
    ASSERT(result == metaFunc<type const&&>::member);                         \
    ASSERT(result == metaFunc<type volatile&&>::member);                      \
    ASSERT(result == metaFunc<type const volatile&&>::member);

#define TYPE_ASSERT_CVQ(metaFunc, member, type, result)                       \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, type, result);                   \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, const type, result);             \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, volatile type, result);          \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, const volatile type, result);

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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
///Example 1: Verify Reference Types
///- - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are (lvalue or rvalue)
// reference types.
//
// Now, we instantiate the 'bsl::is_reference' template for a non-reference
// type, an lvalue reference type, and an rvalue reference type, and assert the
// 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_reference<int>::value);
    ASSERT(true  == bsl::is_reference<int&>::value);
  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ASSERT(true  == bsl::is_reference<int&&>::value);
  #endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standard,
// and may not be supported by all compilers.


      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_reference::value'
        //   Ensure that the static data member 'value' of 'bsl::is_reference'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_reference::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_reference::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_reference::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type.
        //:
        //: 4 'is_reference::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) lvalue or rvalue reference type.
        //:
        //: 5 'is_reference::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_reference::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_reference::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_reference::value'\n"
                            "==========================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_reference, value, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_reference, value, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_reference, value, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_reference, value, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_reference, value, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_reference, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_reference, value, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
                   bsl::is_reference, value, int*,                      false);
        TYPE_ASSERT_CVQ(
                   bsl::is_reference, value, StructTestType*,           false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
                  bsl::is_reference, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                  bsl::is_reference, value, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_SUFFIX(
                  bsl::is_reference, value, FunctionPtrTestType,        false);

        // C-4
        TYPE_ASSERT_CVQ_REF(bsl::is_reference, value, int,  true);
        TYPE_ASSERT_CVQ_REF(
                         bsl::is_reference, value, EnumTestType,         true);
        TYPE_ASSERT_CVQ_REF(
                         bsl::is_reference, value, StructTestType,       true);
        TYPE_ASSERT_CVQ_REF(
                         bsl::is_reference, value, UnionTestType,        true);
        TYPE_ASSERT_CVQ_REF(
                         bsl::is_reference, value, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_REF(
                         bsl::is_reference, value, DerivedClassTestType, true);

        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, int*,                       true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, StructTestType*,            true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, int StructTestType::*,      true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, int StructTestType::* *,    true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, UnionTestType*,             true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, PMD BaseClassTestType::*,   true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, PMD BaseClassTestType::* *, true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, BaseClassTestType*,         true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, DerivedClassTestType*,      true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, Incomplete*,                true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, MethodPtrTestType,          true);
        TYPE_ASSERT_CVQ_REF(
                   bsl::is_reference, value, FunctionPtrTestType,        true);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TYPE_ASSERT_CVQ_RREF(bsl::is_reference, value, int,  true);

        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_reference, value, EnumTestType,         true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_reference, value, StructTestType,       true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_reference, value, UnionTestType,        true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_reference, value, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_RREF(
                         bsl::is_reference, value, DerivedClassTestType, true);

        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, int*,                       true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, StructTestType*,            true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, int StructTestType::*,      true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, int StructTestType::* *,    true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, UnionTestType*,             true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, PMD BaseClassTestType::*,   true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, PMD BaseClassTestType::* *, true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, BaseClassTestType*,         true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, DerivedClassTestType*,      true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, Incomplete*,                true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, MethodPtrTestType,          true);
        TYPE_ASSERT_CVQ_RREF(
                   bsl::is_reference, value, FunctionPtrTestType,        true);
#endif

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (int),    false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (void),   false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (void),   false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (int),    false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (...),    false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (...),    false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (int...), false);

        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&)(int),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&)(void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&)(void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&)(int),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&)(...),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&)(...),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&)(int...),
                               true);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&&)(int), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&&)(void),true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&&)(void),true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&&)(int), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&&)(...), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (&&)(...), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (&&)(int...),
                               true);
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
