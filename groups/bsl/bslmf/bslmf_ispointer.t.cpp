// bslmf_ispointer.t.cpp                                              -*-C++-*-

#include <bslmf_ispointer.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The objects under test are two meta-functions, 'bsl::is_pointer' and
// 'bslmf::IsPointer', that determine whether a template parameter type is a
// pointer type.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsPointer:VALUE
// [ 1] bsl::is_pointer::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

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

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::is_pointer'.
};

typedef int (TestType::*MethodPtrTestType) ();
    // This pointer to non-static function member type is intended to be used
    // during testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_pointer' and 'bslmf::IsPointer'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument for the template parameter 'TYPE' of
    // 'bsl::is_pointer' and 'bslmf::IsPointer'.

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ_SUFFIX(metaFunc, member, type, result)                \
    ASSERT(result == metaFunc<type>::member);                                 \
    ASSERT(result == metaFunc<type const>::member);                           \
    ASSERT(result == metaFunc<type volatile>::member);                        \
    ASSERT(result == metaFunc<type const volatile>::member);

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
    int veryVerbose = argc > 3;

    (void)veryVerbose;

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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a pointer type.
//
// First, we create two 'typedef's -- a pointer type and another type:
//..
        typedef int  MyType;
        typedef int *MyPtrType;
//..
// Now, we instantiate the 'bsl::is_pointer' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
        ASSERT(false == bsl::is_pointer<MyType>::value);
        ASSERT(true == bsl::is_pointer<MyPtrType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsPointer::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsPointer'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsPointer::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitive type.
        //
        //: 2 'IsPointer::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   user-defined type.
        //:
        //: 3 'IsPointer::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer to a (possibly cv-qualified) non-static member.
        //:
        //: 4 'IsPointer::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer to a (possibly cv-qualified) type.
        //
        // Plan:
        //   Verify that 'bsl::IsPointer::VALUE' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::IsPointer::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("\nbslmf::IsPointer::VALUE\n"
                            "\n=======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsPointer, VALUE, int, 0);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsPointer, VALUE, TestType, 0);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsPointer, VALUE, MethodPtrTestType, 0);

        // C-4
        TYPE_ASSERT_CVQ(bslmf::IsPointer, VALUE, int *, 1);
        TYPE_ASSERT_CVQ(bslmf::IsPointer, VALUE, TestType *, 1);
        TYPE_ASSERT_CVQ_SUFFIX(bslmf::IsPointer,
                               VALUE,
                               FunctionPtrTestType,
                               1);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_pointer::value'
        //   Ensure that the static data member 'value' of 'bsl::is_pointer'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //
        //: 2 'is_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_pointer::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) non-static
        //:   member.
        //:
        //: 4 'is_pointer::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer to a (possibly cv-qualified) type.
        //
        // Plan:
        //   Verify that 'bsl::is_pointer::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_pointer::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nbsl::is_pointer::value\n"
                            "\n======================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_pointer, value, int, false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_pointer, value, TestType, false);

        // C-3
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_pointer,
                               value,
                               MethodPtrTestType,
                               false);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_pointer, value, int *, true);
        TYPE_ASSERT_CVQ(bsl::is_pointer, value, TestType *, true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_pointer,
                               value,
                               FunctionPtrTestType,
                               true);

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
