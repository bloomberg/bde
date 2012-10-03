// bslmf_isreference.t.cpp                                            -*-C++-*-
#include <bslmf_isreference.h>

#include <bsls_compilerfeatures.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

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

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

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
    // This pointer to non-static function member type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_reference'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_reference'.

typedef int StructTestType::*PMD;
    // This pointer to data member type is intended to be used for testing as
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
// Note that rvalue reference is a feature introduced in the C++11 standand,
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
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_reference, value, void (int),  false);
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
