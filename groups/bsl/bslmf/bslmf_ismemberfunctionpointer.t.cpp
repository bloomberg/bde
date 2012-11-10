// bslmf_ismemberfunctionpointer.t.cpp                                -*-C++-*-
#include <bslmf_ismemberfunctionpointer.h>

#include <bsls_bsltestutil.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace bsl;
using namespace BloombergLP;

using std::printf;
using std::fprintf;
using std::cerr;
using std::endl;
using std::atoi;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function, 'bsl::is_member_function_pointer',
// which determine whether a template parameter type is a function pointer type
// to (non-static) member function.  Thus, we need to ensure that the value
// returned by this meta-function is correct for each possible category of
// types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_member_function_pointer::value
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
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.
};

union  UnionTestType {
    // This user-defined 'union' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.
};

class  BaseClassTestType {
    // This user-defined base class type is intended to be used during testing
    // as an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.
};

class  DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This function pointer type to non-static member function is intended to
    // be used during testing as an argument for the template parameter 'TYPE'
    // of 'bsl::is_member_function_pointer'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::is_member_function_pointer'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::is_member_function_pointer'.

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
///Example 1: Verify Member Function Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create a user-defined type 'MyStruct':
//..
struct MyStruct
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
// Now, we create two 'typedef's -- a function pointer type and a member
// function pointer type:
//..
    typedef int (MyStruct::*MyStructMethodPtr) ();
    typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_function_pointer' template for
// each of the 'typedef's and assert the 'value' static data member of each
// instantiation:
//..
    ASSERT(false == bsl::is_member_function_pointer<MyFunctionPtr    >::value);
    ASSERT(true  == bsl::is_member_function_pointer<MyStructMethodPtr>::value);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_member_function_pointer::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_member_function_pointer' instantiations having various
        //   (template parameter) 'TYPES' has the correct value.
        //
        // Concerns:
        //: 1 'is_member_function_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) primitive type.
        //:
        //: 2 'is_member_function_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) user-defined type.
        //:
        //: 3 'is_member_function_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) pointer type other than a non-static
        //:   member function pointer.
        //:
        //: 4 'is_member_function_pointer::value' is 'true' when 'TYPE' is a
        //:   (possibly cv-qualified) non-static member function pointer type.
        //:
        //: 5 'is_member_function_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_member_function_pointer::value' has the
        //   correct value for each (template parameter) 'TYPE' in the
        //   concerns.
        //
        // Testing:
        //   bsl::is_member_function_pointer::value
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::is_member_function_pointer\n"
                            "===============================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_function_pointer, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_function_pointer, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_member_function_pointer, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF   (
                 bsl::is_member_function_pointer, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF   (
                 bsl::is_member_function_pointer, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, Incomplete,           false);
        TYPE_ASSERT_CVQ_REF   (
                 bsl::is_member_function_pointer, Incomplete,           false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (
                 bsl::is_member_function_pointer, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                 bsl::is_member_function_pointer, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (
                 bsl::is_member_function_pointer, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, int*,                       false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
           bsl::is_member_function_pointer, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                  bsl::is_member_function_pointer, FunctionPtrTestType, false);

        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, int*,                       false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, FunctionPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
           bsl::is_member_function_pointer, MethodPtrTestType,          false);

        // C-4
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_function_pointer, MethodPtrTestType,   true);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(
                          bsl::is_member_function_pointer, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(
                          bsl::is_member_function_pointer, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                          bsl::is_member_function_pointer, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                          bsl::is_member_function_pointer, void (int),  false);

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

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
