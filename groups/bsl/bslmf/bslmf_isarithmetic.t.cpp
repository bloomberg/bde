// bslmf_isarithmetic.t.cpp                                           -*-C++-*-
#include <bslmf_isarithmetic.h>

#include <bsls_bsltestutil.h>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function, 'bsl::is_arithmetic', which
// determine whether a template parameter type is a arithmetic type.  Thus, we
// need to ensure that the value returned by this meta-functions is correct for
// each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_arithmetic::value
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

enum   EnumTestType {
    // This user-defined 'enum' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

union  UnionTestType {
    // This user-defined 'union' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

class  BaseClassTestType {
    // This user-defined base class type is intended to be used during testing
    // as an argument for the template parameter 'TYPE' of
    // 'bsl::is_arithmetic'.
};

class  DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_arithmetic'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_arithmetic'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_arithmetic'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::is_arithmetic'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_arithmetic'.

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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

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
///Example 1: Verify Arithmetic Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a arithmetic
// type.
//
// Now, we instantiate the 'bsl::is_arithmetic' template for both a
// non-arithmetic type and a arithmetic type, and assert the 'value' static
// data member of each instantiation:
//..
    ASSERT(false == bsl::is_arithmetic<int&>::value);
    ASSERT(true  == bsl::is_arithmetic<int >::value);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_arithmetic::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_arithmetic' instantiations having various (template
        //   parameter) 'TYPES' has the correct value.
        //
        // Concerns:
        //: 1 'is_arithmetic::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type other than arithmetic type.
        //:
        //: 2 'is_arithmetic::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) arithmetic type.
        //:
        //: 3 'is_arithmetic::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 4 'is_arithmetic::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type.
        //:
        //: 5 'is_arithmetic::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) reference type.
        //:
        //: 6 'is_arithmetic::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_arithmetic::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_arithmetic::value
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::is_arithmetic::value\n"
                            "=========================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, void, false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, char,          true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, unsigned char, true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, int,           true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, float,         true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, double,        true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, unsigned int,  true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, long,          true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, long long,     true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, long double,   true);
        TYPE_ASSERT_CVQ_SUFFIX(
                      bsl::is_arithmetic, value, unsigned long,          true);
        TYPE_ASSERT_CVQ_SUFFIX(
                      bsl::is_arithmetic, value, unsigned long long,     true);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_arithmetic, value, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_arithmetic, value, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_arithmetic, value, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_arithmetic, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_arithmetic, value, DerivedClassTestType, false);

        // C-4
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, int*,                       false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
                 bsl::is_arithmetic, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_arithmetic, value, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_arithmetic, value, FunctionPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQ_REF(bsl::is_arithmetic, value, int,  false);
        TYPE_ASSERT_CVQ_REF(
                       bsl::is_arithmetic, value, EnumTestType,         false);
        TYPE_ASSERT_CVQ_REF(
                       bsl::is_arithmetic, value, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF(
                       bsl::is_arithmetic, value, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF(
                       bsl::is_arithmetic, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF(
                       bsl::is_arithmetic, value, DerivedClassTestType, false);

        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, int*,                       false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_REF(
                 bsl::is_arithmetic, value, FunctionPtrTestType,        false);

        // C-6
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_arithmetic, value, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_arithmetic, value, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_arithmetic, value, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_arithmetic, value, void (int),  false);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
