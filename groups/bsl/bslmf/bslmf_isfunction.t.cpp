// bslmf_isfunction.t.cpp                                             -*-C++-*-
#include <bslmf_isfunction.h>

#include <bsls_bsltestutil.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_function', that
// determines whether a template parameter type is a function type.  Thus, we
// need to ensure that the value returned by this meta-function is correct for
// each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_function::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
    // template parameter 'TYPE' of 'bsl::is_function'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_function'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_function'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.

typedef void VoidRetNoParamFunctionTestType();
    // This function type having 'void' return type and no parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int IntRetNoParamFunctionTestType();
    // This function type having 'int' return type and no parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int IntRetParamFunctionTestType(int, int);
    // This function type having 'int' return type and 'int' parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.

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
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for a non-function type
// and a function type, and assert the 'value' static data member of each
// instantiation:
//..
    ASSERT(false == bsl::is_function<int>::value);
    ASSERT(true  == bsl::is_function<int (int)>::value);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_function::value'
        //   Ensure that the static data member 'value' of 'bsl::is_function'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer or pointer-to-member type.
        //:
        //: 4 'is_function::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_function::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_function::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_function::value'\n"
                            "=========================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, EnumTestType,         false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_function, int*,                       false);
        TYPE_ASSERT_CVQ(bsl::is_function, StructTestType*,            false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(bsl::is_function, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(bsl::is_function, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(bsl::is_function, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(bsl::is_function, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(bsl::is_function, int*,                     false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, StructTestType*,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::* *,  false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, UnionTestType*,           false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_REF(
                          bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, BaseClassTestType*,       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, DerivedClassTestType*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, Incomplete*,              false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, MethodPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, FunctionPtrTestType,      false);

        // C-4
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int  (int),  true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, void (void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int  (void), true);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, void (int),  true);
        TYPE_ASSERT_CVQ_PREFIX(
                       bsl::is_function, VoidRetNoParamFunctionTestType, true);
        TYPE_ASSERT_CVQ_PREFIX(
                       bsl::is_function, IntRetNoParamFunctionTestType,  true);
        TYPE_ASSERT_CVQ_PREFIX(
                       bsl::is_function, IntRetParamFunctionTestType,    true);
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
