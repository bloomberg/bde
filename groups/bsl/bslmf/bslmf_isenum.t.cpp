// bslmf_isenum.t.cpp                                                 -*-C++-*-
#include <bslmf_isenum.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_enum' and
// 'bslmf::IsEnum', that determine whether a template parameter type is an
// enumerated type.  Thus, we need to ensure that the value returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] bslmf::IsEnum::VALUE
// [ 1] bsl::is_enum::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.
    ENUM_TEST_VALUE0 = 0,
    ENUM_TEST_VALUE1,
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_enum'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_enum'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

struct ConvertToIntTestType {
    // This 'struct' type, having a conversion operator to 'int', is intended
    // to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_enum'.  This type should not be determined to be an enumerated
    // type.

    operator int() { return 0; }
};

struct ConvertToEnumTestType {
    // This 'struct' type, having a conversion operator to 'EnumTestType', is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_enum'.  This type should not be determined to be an enumerated
    // type.

    operator EnumTestType() { return ENUM_TEST_VALUE0; }
};

struct ConvertToAnyType {
    template <class T>
    operator T() { return T(); }
};

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE, result)       \
    ASSERT(result == META_FUNC<TYPE>::value);                 \
    ASSERT(result == META_FUNC<const TYPE>::value);           \
    ASSERT(result == META_FUNC<volatile TYPE>::value);        \
    ASSERT(result == META_FUNC<const volatile TYPE>::value);

#define TYPE_ASSERT_CVQ_SUFFIX(META_FUNC, TYPE, result)       \
    ASSERT(result == META_FUNC<TYPE>::value);                 \
    ASSERT(result == META_FUNC<TYPE const>::value);           \
    ASSERT(result == META_FUNC<TYPE volatile>::value);        \
    ASSERT(result == META_FUNC<TYPE const volatile>::value);

#define TYPE_ASSERT_CVQ_REF(META_FUNC, TYPE, result)           \
    ASSERT(result == META_FUNC<TYPE&>::value);                 \
    ASSERT(result == META_FUNC<TYPE const&>::value);           \
    ASSERT(result == META_FUNC<TYPE volatile&>::value);        \
    ASSERT(result == META_FUNC<TYPE const volatile&>::value);

#define TYPE_ASSERT_CVQ(META_FUNC, TYPE, result)                     \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE, result);                 \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const, result);           \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE volatile, result);        \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const volatile, result);  \

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Enumerated Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are 'enum' types.
//
// First, we create an enumerated type, 'MyEnum', and a class type, 'MyClass':
//..
    enum MyEnum { MY_ENUMERATOR = 5 };
    class MyClass { explicit MyClass(MyEnum); };
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

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

// Now, we instantiate the 'bsl::is_enum' template for both types we defined
// previously, and assert the 'value' static data member of each instantiation:
//..
    ASSERT(true  == bsl::is_enum<MyEnum>::value);
    ASSERT(false == bsl::is_enum<MyClass>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsEnum::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsEnum'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitive type.
        //:
        //: 2 'IsEnum::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   'enum' type, and is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   reference to an enumerated type.
        //:
        //: 3 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'class', 'struct', or 'union' type.
        //:
        //: 4 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer or pointer-to-member type.
        //:
        //: 5 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   user-defined type having conversions to integral or enumerated
        //:   type, or a reference to such a user-defined type.
        //
        // Plan:
        //   Verify that 'bslmf::IsEnum::VALUE' has the correct value for each
        //   (template parameter) 'TYPE' in the concerns.  (C-1..5)
        //
        // Testing:
        //   bslmf::IsEnum::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("'bslmf::IsEnum::VALUE'\n"
                            "======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, int,  0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, int,  0);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, EnumTestType, 1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, EnumTestType, 0);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, StructTestType,       0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, StructTestType,       0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, UnionTestType,        0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, UnionTestType,        0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, Incomplete,           0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, Incomplete,           0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, BaseClassTestType,    0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, BaseClassTestType,    0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, DerivedClassTestType, 0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, DerivedClassTestType, 0);

        // C-4
        TYPE_ASSERT_CVQ(bslmf::IsEnum, int*,                       0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, StructTestType*,            0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, UnionTestType*,             0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsEnum, Incomplete*,                0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, MethodPtrTestType,   0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, FunctionPtrTestType, 0);

        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, int*,                       0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, StructTestType*,            0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, UnionTestType*,             0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, Incomplete*,                0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, MethodPtrTestType,          0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsEnum, FunctionPtrTestType,        0);

        // C-5
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, ConvertToIntTestType,  0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, ConvertToIntTestType,  0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, ConvertToEnumTestType, 0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, ConvertToEnumTestType, 0);

        // can't use TYPE_ASSERT_CVQ because it adds volatile and
        // ConvertibleToAny doesn't have a volatile operator()
        ASSERT(! bslmf::IsEnum<ConvertToAnyType>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType const>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType &>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType const &>::value);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_enum::value'
        //   Ensure that the static data member 'value' of 'bsl::is_enum'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_enum::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'enum' type, and is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) reference to an enumerated type.
        //:
        //: 3 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) 'class', 'struct', or 'union' type.
        //:
        //: 4 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer or pointer-to-member type.
        //:
        //: 5 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type having conversions to integral or
        //:   enumerated type, or a reference to such a user-defined type.
        //
        //: 6 'is_enum::value' is 'false' when 'TYPE' is a function or function
        //:   reference type.
        //
        //: 7 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) array type.
        //
        //: 8 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) void type.
        //
        // Plan:
        //   Verify that 'bsl::is_enum::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.  (C-1..5)
        //
        // Testing:
        //   bsl::is_enum::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_enum::value'\n"
                            "=====================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, EnumTestType, true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, EnumTestType, false);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, Incomplete,           false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, Incomplete,           false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, DerivedClassTestType, false);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_enum, int*,                       false);
        TYPE_ASSERT_CVQ(bsl::is_enum, StructTestType*,            false);
        TYPE_ASSERT_CVQ(bsl::is_enum, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(bsl::is_enum, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(bsl::is_enum, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(bsl::is_enum, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(bsl::is_enum, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(bsl::is_enum, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(bsl::is_enum, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(bsl::is_enum, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(bsl::is_enum, int*,                       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_enum, FunctionPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, ConvertToIntTestType,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, ConvertToIntTestType,  false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, ConvertToEnumTestType, false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, ConvertToEnumTestType, false);

        // can't use TYPE_ASSERT_CVQ because it adds volatile and
        // ConvertibleToAny doesn't have a volatile operator()
        ASSERT(! bsl::is_enum<ConvertToAnyType>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType const>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType &>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType const &>::value);

        // C-6
        ASSERT(! bsl::is_enum<int(int)>::value);
        ASSERT(! bsl::is_enum<void(...)>::value);

        ASSERT(! bsl::is_enum<void()>::value);
        ASSERT(! bsl::is_enum<int(char, float...)>::value);
        ASSERT(! bsl::is_enum<void(&)()>::value);
        ASSERT(! bsl::is_enum<int(&)(char, float...)>::value);

        // C-8

        // These tests dp not use the test macros above, as you need to use a
        // different syntax to correctly add a cv-qualifier, or make a pointer
        // or reference to, array types that does not fall out of the simple
        // textual replacement of a macro.  We could, instead, use the 'add_*'
        // metafunctions. introducing a further component depenency to the
        // levelization of this package.

        ASSERT(! bsl::is_enum<int[2]>::value);
        ASSERT(! bsl::is_enum<const int[2]>::value);
        ASSERT(! bsl::is_enum<volatile int[2]>::value);
        ASSERT(! bsl::is_enum<const volatile int[2]>::value);

        ASSERT(! bsl::is_enum<int[4][2]>::value);
        ASSERT(! bsl::is_enum<const int[4][2]>::value);
        ASSERT(! bsl::is_enum<volatile int[4][2]>::value);
        ASSERT(! bsl::is_enum<const volatile int[4][2]>::value);

        ASSERT(! bsl::is_enum<EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[2]>::value);

        ASSERT(! bsl::is_enum<EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[4][2]>::value);

#if !defined(BSLS_PLATFORM_CMP_IBM)                                     \
 &&!(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700)
        // The IBM xlC compiler does not handle arrays of unknown bounds as
        // template type parameters.  MSVC has problems with references to
        // arrays of unknown bound that fall out of the template metaprograms
        // used to implement this trait.
        ASSERT(! bsl::is_enum<int[]>::value);
        ASSERT(! bsl::is_enum<const int[]>::value);
        ASSERT(! bsl::is_enum<volatile int[]>::value);
        ASSERT(! bsl::is_enum<const volatile int[]>::value);

        ASSERT(! bsl::is_enum<int[][2]>::value);
        ASSERT(! bsl::is_enum<const int[][2]>::value);
        ASSERT(! bsl::is_enum<volatile int[][2]>::value);
        ASSERT(! bsl::is_enum<const volatile int[][2]>::value);

        ASSERT(! bsl::is_enum<EnumTestType[]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[]>::value);

        ASSERT(! bsl::is_enum<EnumTestType[][2]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[][2]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[][2]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[][2]>::value);
#endif

        // C-8
        ASSERT(! bsl::is_enum<void>::value);
        ASSERT(! bsl::is_enum<const void>::value);
        ASSERT(! bsl::is_enum<volatile void>::value);
        ASSERT(! bsl::is_enum<const volatile void>::value);

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
