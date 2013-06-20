// bslmf_isarithmetic.t.cpp                                           -*-C++-*-
#include <bslmf_isarithmetic.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_arithmetic', that
// determines whether a template parameter type is an arithmetic type.  Thus,
// we need to ensure that the value returned by this meta-function is correct
// for each possible category of types.
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

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_arithmetic'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_arithmetic'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_arithmetic'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_arithmetic'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_arithmetic'.

typedef int  FunctionIntIntTestType   (int);
typedef void FunctionVoidVoidTestType (void);
typedef int  FunctionIntVoidTestType  (void);
typedef void FunctionVoidIntTestType  (int);
    // These function types are intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_arithmetic'.

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
// Suppose that we want to assert whether a set of types are arithmetic types.
//
// Now, we instantiate the 'bsl::is_arithmetic' template for these types, and
// assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_arithmetic<int& >::value);
    ASSERT(false == bsl::is_arithmetic<int *>::value);
    ASSERT(true  == bsl::is_arithmetic<int  >::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_arithmetic::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_arithmetic' instantiations having various (template
        //   parameter) 'TYPE's has the correct value.
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

        if (verbose) printf("'bsl::is_arithmetic::value'\n"
                            "===========================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, void,         false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, bool,          true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, char,          true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, unsigned char, true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, wchar_t,       true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, short,         true);
        TYPE_ASSERT_CVQ_SUFFIX(
                              bsl::is_arithmetic, value, unsigned short, true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, int,           true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, unsigned int,  true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, float,         true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_arithmetic, value, double,        true);
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
        TYPE_ASSERT_CVQ_REF(bsl::is_arithmetic, value, int,             false);
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
        TYPE_ASSERT_CVQ_PREFIX(
                 bsl::is_arithmetic, value, FunctionIntIntTestType,     false);
        TYPE_ASSERT_CVQ_PREFIX(
                 bsl::is_arithmetic, value, FunctionVoidVoidTestType,   false);
        TYPE_ASSERT_CVQ_PREFIX(
                 bsl::is_arithmetic, value, FunctionIntVoidTestType,    false);
        TYPE_ASSERT_CVQ_PREFIX(
                 bsl::is_arithmetic, value, FunctionVoidIntTestType,    false);

      } break;
      default: {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
