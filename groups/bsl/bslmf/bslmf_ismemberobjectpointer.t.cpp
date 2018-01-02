// bslmf_ismemberobjectpointer.t.cpp                                  -*-C++-*-
#include <bslmf_ismemberobjectpointer.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bsl::is_member_object_pointer', that determines whether a template
// parameter type is a pointer to (non-static) member object type.  Thus, we
// need to ensure that the value returned by this meta-function is correct for
// each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_member_object_pointer::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consquently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_CMP_GNU)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180) // cv-qualifiers meaningless on function types
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of
    // 'bsl::is_member_object_pointer'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.

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
///Example 1: Verify Member Object Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are pointers to member
// object types.
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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

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

// Now, we create a 'typedef' for a member object pointer type:
//..
    typedef int MyStruct::* DataMemPtr;
//..
// Finally, we instantiate the 'bsl::is_member_object_pointer' template for a
// non-member data type and the 'MyStructDataPtr' type, and assert the 'value'
// static data member of each instantiation:
//..
    ASSERT(false == bsl::is_member_object_pointer<int*>::value);
    ASSERT(true  == bsl::is_member_object_pointer<DataMemPtr>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_member_object_pointer::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_member_object_pointer' instantiations having various
        //   (template parameter) 'TYPE's has the correct value.
        //
        // Concerns:
        //: 1 'is_member_object_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) primitive type.
        //:
        //: 2 'is_member_object_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) user-defined type.
        //:
        //: 3 'is_member_object_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) pointer type other than pointer
        //:   non-static data member type.
        //:
        //: 4 'is_member_object_pointer::value' is 'true' when 'TYPE' is a
        //:   (possibly cv-qualified) pointer to non-static data member type.
        //:
        //: 5 'is_member_object_pointer::value' is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) function type.
        //
        // Plan:
        //   Verify that 'bsl::is_member_object_pointer::value' has the
        //   correct value for each (template parameter) 'TYPE' in the
        //   concerns.
        //
        // Testing:
        //   bsl::is_member_object_pointer::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_member_object_pointer::value'\n"
                            "======================================\n");

        // C-1
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_object_pointer, void, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_member_object_pointer, int,  false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_member_object_pointer, int,  false);

        // C-2
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, EnumTestType,         false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, StructTestType,       false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_object_pointer, StructTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, UnionTestType,        false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_object_pointer, UnionTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, Incomplete,           false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_object_pointer, Incomplete,           false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_object_pointer, BaseClassTestType,    false);
        TYPE_ASSERT_CVQ_SUFFIX(
                   bsl::is_member_object_pointer, DerivedClassTestType, false);
        TYPE_ASSERT_CVQ_REF   (
                   bsl::is_member_object_pointer, DerivedClassTestType, false);

        // C-3
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, int*,                       false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, StructTestType*,            false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, UnionTestType*,             false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ(
             bsl::is_member_object_pointer, Incomplete*,                false);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_member_object_pointer, FunctionPtrTestType, false);
        TYPE_ASSERT_CVQ_SUFFIX(
                    bsl::is_member_object_pointer, MethodPtrTestType,   false);

        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, int*,                       false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, StructTestType*,            false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, int StructTestType::*,      false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, int StructTestType::* *,    false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, UnionTestType*,             false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, BaseClassTestType*,         false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, DerivedClassTestType*,      false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, Incomplete*,                false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, FunctionPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(
             bsl::is_member_object_pointer, MethodPtrTestType,          false);

        // C-4
        TYPE_ASSERT_CVQ(
                   bsl::is_member_object_pointer, int StructTestType::*, true);
        TYPE_ASSERT_CVQ(
                bsl::is_member_object_pointer, PMD BaseClassTestType::*, true);


        // C-5
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_object_pointer, int  (int),  false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_object_pointer, void (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_object_pointer, int  (void), false);
        TYPE_ASSERT_CVQ_PREFIX(
                            bsl::is_member_object_pointer, void (int),  false);

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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
