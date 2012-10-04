// bslmf_isfundamental.t.cpp                                          -*-C++-*-

#include <bslmf_isfundamental.h>

//#include <bsls_platform.h>
//#include <bsls_types.h>
#include <bsls_bsltestutil.h>

#include <cstdio>
#include <cstdlib>     // atoi()

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_fundamental'
// and 'bslmf::IsFundamental', that determine whether a template parameter type
// is a fundamental type.  Thus, we need to ensure that the value returned by
// these meta-functions are correct for each possible category of types.  Since
// the two meta-functions are functionally equivalent, we will use the same set
// of types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsFundamental::VALUE
// [ 1] bsl::is_fundamental::value
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

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_fundamental'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_fundamental'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_fundamental'.

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
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Fundamental Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are fundamental types.
//
// Now, we instantiate the 'bsl::is_fundamental' template for a couple of
// non-fundamental and fundamental types, and assert the 'value' static data
// member of each instantiation:
//..
    ASSERT(true  == bsl::is_fundamental<int>::value);
    ASSERT(false == bsl::is_fundamental<int&>::value);
    ASSERT(true  == bsl::is_fundamental<long long>::value);
    ASSERT(false == bsl::is_fundamental<long long*>::value);
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
        //: 1 'IsFundamental::VALUE' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'IsFundamental::VALUE' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'IsFundamental::VALUE' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) type.
        //:
        //: 4 'IsFundamental::VALUE' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
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
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, char,                   1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, signed char,            1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, unsigned char,          1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, wchar_t,                1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, short int,              1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, unsigned short int,     1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, int,                    1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, unsigned int,           1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, long int,               1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, unsigned long int,      1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, float,                  1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, double,                 1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, long double,            1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, void,                   1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, long long int,          1);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bslmf::IsFundamental, VALUE, unsigned long long int, 1);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, EnumTestType,         0);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, StructTestType,       0);
        TYPE_ASSERT_CVQ_REF   (
                         bslmf::IsFundamental, VALUE, StructTestType,       0);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, UnionTestType,        0);
        TYPE_ASSERT_CVQ_REF   (
                         bslmf::IsFundamental, VALUE, UnionTestType,        0);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, Incomplete,           0);
        TYPE_ASSERT_CVQ_REF   (
                         bslmf::IsFundamental, VALUE, Incomplete,           0);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, BaseClassTestType,    0);
        TYPE_ASSERT_CVQ_REF   (
                         bslmf::IsFundamental, VALUE, BaseClassTestType,    0);
        TYPE_ASSERT_CVQ_SUFFIX(
                         bslmf::IsFundamental, VALUE, DerivedClassTestType, 0);
        TYPE_ASSERT_CVQ_REF   (
                         bslmf::IsFundamental, VALUE, DerivedClassTestType, 0);

        // C-3
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, int*,                       0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, StructTestType*,            0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, UnionTestType*,             0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, Incomplete*,                0);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bslmf::IsFundamental, VALUE, FunctionPtrTestType,        0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, int*,                       0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, StructTestType*,            0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, UnionTestType*,             0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, Incomplete*,                0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, FunctionPtrTestType,        0);
        TYPE_ASSERT_CVQ_REF(
                   bslmf::IsFundamental, VALUE, MethodPtrTestType,          0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ(
                   bslmf::IsFundamental, VALUE, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bslmf::IsFundamental, VALUE, MethodPtrTestType,          0);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsFundamental, VALUE, int  (int),  0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsFundamental, VALUE, void (void), 0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsFundamental, VALUE, int  (void), 0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsFundamental, VALUE, void (int),  0);

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
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) type.
        //:
        //: 4 'is_fundamental::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_fundamental::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_fundamental::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_fundamental::value'\n"
                            "============================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, char,                    true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, signed char,             true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, unsigned char,           true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, wchar_t,                 true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, short int,               true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, unsigned short int,      true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, int,                     true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, unsigned int,            true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, long int,                true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, unsigned long int,       true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, float,                   true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, double,                  true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, long double,             true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, void,                    true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, long long int,           true);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_fundamental, value, unsigned long long int,  true);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                           bsl::is_fundamental, value, EnumTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                           bsl::is_fundamental, value, StructTestType,  false);
        TYPE_ASSERT_CVQ_REF   (
                           bsl::is_fundamental, value, StructTestType,  false);
        TYPE_ASSERT_CVQ_SUFFIX(
                           bsl::is_fundamental, value, UnionTestType,   false);
        TYPE_ASSERT_CVQ_REF   (
                           bsl::is_fundamental, value, UnionTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(
                           bsl::is_fundamental, value, Incomplete,      false);
        TYPE_ASSERT_CVQ_REF   (
                           bsl::is_fundamental, value, Incomplete,      false);
        TYPE_ASSERT_CVQ_SUFFIX(
                      bsl::is_fundamental, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (
                      bsl::is_fundamental, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                      bsl::is_fundamental, value, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (
                      bsl::is_fundamental, value, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, int*,                       false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
                bsl::is_fundamental, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                bsl::is_fundamental, value, FunctionPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, int*,                       false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, FunctionPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
                bsl::is_fundamental, value, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ(
                  bsl::is_fundamental, value, int StructTestType::*,    false);
        TYPE_ASSERT_CVQ(
                  bsl::is_fundamental, value, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_SUFFIX(
                  bsl::is_fundamental, value, MethodPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_fundamental, value, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_fundamental, value, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_fundamental, value, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_fundamental, value, void (int),  false);

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
