// bslmf_isenum.t.cpp                                                 -*-C++-*-

#include <bslmf_isenum.h>

#include <bsls_bsltestutil.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

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
// [ 2] BloombergLP::bslmf::IsEnum::VALUE
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
    // This pointer type to non-static function member is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_enum'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

typedef int StructTestType::* PMD;
    // This pointer type to data member is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

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
// Suppose that we want to assert whether a set of types are enum types.
//
// First, we create an enumerated type, 'MyEnum', and a non-enumerated class
// type, 'MyClass':
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
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

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
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitive type.
        //:
        //: 2 'IsEnum::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   'enum' type.
        //:
        //: 3 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'class', 'struct', or 'union' type.
        //:
        //: 4 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer type.
        //
        // Plan:
        //   Verify that 'bsl::IsEnum::VALUE' has the correct value for each
        //   (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bslmf::IsEnum::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf::IsEnum\n"
                            "=============\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, int,  0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsEnum, int,  0);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsEnum, EnumTestType, 1);

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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_enum::value'
        //   Ensure that the static data member 'value' of 'bsl::is_enum'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_enum::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'enum' type.
        //:
        //: 3 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) 'class', 'struct', or 'union' type.
        //:
        //: 4 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type.
        //
        // Plan:
        //   Verify that 'bsl::is_enum::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_enum::value
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::is_enum\n"
                            "============\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_enum, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_enum, EnumTestType, true);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
