// bslmf_ismemberpointer.t.cpp                                        -*-C++-*-
#include <bslmf_ismemberpointer.h>

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
// The object under test is a meta-function, 'bsl::is_member_pointer', which
// determine whether a template parameter type is a pointer type to non-static
// data member (object or function).  Thus, we need to ensure that the value
// returned by this meta-functions is correct for each possible category of
// types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_member_pointer::value
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

#define TYPE_ASSERT_CVQ(metaFunc, member, type, result)                       \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, type, result);                   \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, const type, result);             \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, volatile type, result);          \
    TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, const volatile type, result);

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Verify Member Pointer Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create a user-defined type 'MyStruct':
//..
    struct MyStruct
    {
        // DATA
        int memData;  // a member data
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
// Now, we create three 'typedef's -- a data member object pointer, a member
// function pointer type and a general function pointer type:
//..
    typedef int MyStruct::* DataMemPtr;
    typedef int (MyStruct::*MyStructMethodPtr) ();
    typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_pointer' template for various
// types and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_member_pointer<int*>::value);
    ASSERT(false == bsl::is_member_pointer<MyFunctionPtr>::value);
    ASSERT(true  == bsl::is_member_pointer<DataMemPtr>::value);
    ASSERT(true  == bsl::is_member_pointer<MyStructMethodPtr>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_member_pointer::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_member_pointer' instantiations having various (template
        //   parameter) 'TYPE' has the correct value.
        //
        // Concerns:
        //: 1 'is_member_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_member_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_member_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type other than a non-static
        //:   data member pointer.
        //:
        //: 4 'is_member_pointer::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer type to non-static data members.
        //:
        //: 5 'is_member_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_member_pointer::value' has the
        //   correct value for each (template parameter) 'TYPE' in the
        //   concerns.
        //
        // Testing:
        //   bsl::is_member_pointer::value
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::is_member_pointer\n"
                            "======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_pointer, value, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_pointer, value, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_member_pointer, value, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_member_pointer, value, EnumTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_member_pointer, value, StructTestType,  false);
        TYPE_ASSERT_CVQ_REF   (
                        bsl::is_member_pointer, value, StructTestType,  false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_member_pointer, value, UnionTestType,   false);
        TYPE_ASSERT_CVQ_REF   (
                        bsl::is_member_pointer, value, UnionTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX(
                        bsl::is_member_pointer, value, Incomplete,      false);
        TYPE_ASSERT_CVQ_REF   (
                        bsl::is_member_pointer, value, Incomplete,      false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_pointer, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_pointer, value, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_pointer, value, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_pointer, value, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, int*,                       false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_pointer, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
             bsl::is_member_pointer, value, FunctionPtrTestType,        false);

        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, int*,                       false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, FunctionPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_pointer, value, MethodPtrTestType,          false);

        // C-4
        TYPE_ASSERT_CVQ(
                   bsl::is_member_pointer, value, int StructTestType::*, true);
        TYPE_ASSERT_CVQ(
                bsl::is_member_pointer, value, PMD BaseClassTestType::*, true);
        TYPE_ASSERT_CVQ_SUFFIX(
                       bsl::is_member_pointer, value, MethodPtrTestType, true);

        // C-5
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_pointer, value, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_pointer, value, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_pointer, value, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_pointer, value, void (int),  false);

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
