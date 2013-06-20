// bslmf_isclass.t.cpp                                                -*-C++-*-
#include <bslmf_isclass.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>     // 'printf'
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_class' and
// 'bslmf::IsClass', that determine whether a template parameter type is a
// class type.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] bslmf::IsClass::VALUE
// [ 1] bsl::is_class::value
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
    // template parameter 'TYPE' of 'bsl::is_class'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_class'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_class'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_class'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_class'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_class'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_class'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_class'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_class'.

typedef int  FunctionIntIntTestType   (int);
typedef void FunctionVoidVoidTestType (void);
typedef int  FunctionIntVoidTestType  (void);
typedef void FunctionVoidIntTestType  (int);
    // These function types are intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_class'.

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
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are class types.
//
// First, we create a class type 'MyClass':
//..
    class MyClass
    {
    };
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

// Now, we instantiate the 'bsl::is_class' template for both a non-class type
// and the defined type 'MyClass', and assert the 'value' static data member of
// each instantiation:
//..
    ASSERT(false == bsl::is_class<int>::value);
    ASSERT(true  == bsl::is_class<MyClass>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsClass::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsClass'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitive type.
        //:
        //: 2 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'enum' type.
        //:
        //: 3 'IsClass::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   'class', 'struct', or 'union' type.
        //:
        //: 4 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer or pointer-to-member type.
        //:
        //: 5 'IsClass::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   function type.
        //
        // Plan:
        //   Verify that 'bslmf::IsClass::VALUE' has the correct value for each
        //   (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bslmf::IsClass::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("'bslmf::IsClass::VALUE'\n"
                            "=======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, void, 0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, int,  0);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, int,  0);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, EnumTestType, 0);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, StructTestType,       1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, StructTestType,       0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, UnionTestType,        1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, UnionTestType,        0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, Incomplete,           1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, Incomplete,           0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, BaseClassTestType,    1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, BaseClassTestType,    0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, DerivedClassTestType, 1);
        TYPE_ASSERT_CVQ_REF   (bslmf::IsClass, DerivedClassTestType, 0);

        // C-4
        TYPE_ASSERT_CVQ(bslmf::IsClass, int*,                       0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, StructTestType*,            0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, UnionTestType*,             0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ(bslmf::IsClass, Incomplete*,                0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, MethodPtrTestType,   0);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsClass, FunctionPtrTestType, 0);

        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int*,                       0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, StructTestType*,            0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int StructTestType::*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, int StructTestType::* *,    0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, UnionTestType*,             0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, PMD BaseClassTestType::*,   0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, PMD BaseClassTestType::* *, 0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, BaseClassTestType*,         0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, DerivedClassTestType*,      0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, Incomplete*,                0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, MethodPtrTestType,          0);
        TYPE_ASSERT_CVQ_REF(bslmf::IsClass, FunctionPtrTestType,        0);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, FunctionIntIntTestType,   0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, FunctionVoidVoidTestType, 0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, FunctionIntVoidTestType,  0);
        TYPE_ASSERT_CVQ_PREFIX(bslmf::IsClass, FunctionVoidIntTestType,  0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_class::value'
        //   Ensure that the static data member 'value' of 'bsl::is_class'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) 'enum' type.
        //:
        //: 3 'is_class::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'class', 'struct', or 'union' type.
        //:
        //: 4 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer or pointer-to-member type.
        //:
        //: 5 'is_class::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_class::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_class::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_class::value'\n"
                            "======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, EnumTestType, false);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, StructTestType,       true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, UnionTestType,        true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, Incomplete,           true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, Incomplete,           false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, BaseClassTestType,    true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, DerivedClassTestType, true);
        TYPE_ASSERT_CVQ_REF   (bsl::is_class, DerivedClassTestType, false);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_class, int*,                       false);
        TYPE_ASSERT_CVQ(bsl::is_class, StructTestType*,            false);
        TYPE_ASSERT_CVQ(bsl::is_class, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(bsl::is_class, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(bsl::is_class, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(bsl::is_class, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(bsl::is_class, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(bsl::is_class, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(bsl::is_class, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(bsl::is_class, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, MethodPtrTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_class, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(bsl::is_class, int*,                       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, MethodPtrTestType,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_class, FunctionPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, FunctionIntIntTestType,   false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, FunctionVoidVoidTestType, false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, FunctionIntVoidTestType,  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_class, FunctionVoidIntTestType,  false);
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
