// bslma_isstdallocator.t.cpp                                         -*-C++-*-

#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>

#include <bsls_bsltestutil.h>

#include <cassert>     // assert
#include <stdio.h>     // 'printf'
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::IsStdAllocator' and
// a template variable 'bsl::IsStdAllocator_v', that determine whether a
// template parameter type meets the requirements for an allocator, as
// specified (loosely) in [container.requirements.general].  Thus, we need to
// ensure that the values returned by the meta-function are correct for each
// possible category of types.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::IsStdAllocator::value
// [ 1] bsl::IsStdAllocator_v
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
// that are not addressed by this traits component.  Consequently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::IsStdAllocator'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::IsStdAllocator'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::IsStdAllocator'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::IsStdAllocator'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::IsStdAllocator'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::IsStdAllocator'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::IsStdAllocator'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::IsStdAllocator'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::IsStdAllocator'.

}  // close unnamed namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#   define TYPE_ASSERT_V_SAME(type)                                           \
       ASSERT(bsl::IsStdAllocator<type>::value == bsl::IsStdAllocator_v<type>)
    // 'ASSERT' that 'IsStdAllocator_v' has the same value as
    // 'IsStdAllocator::value'.
#else
#   define TYPE_ASSERT_V_SAME(type)
#endif

#define TYPE_ASSERT_V(META_FUNC, TYPE, result)                                \
    ASSERT(result == META_FUNC<TYPE>::value);                                 \
    TYPE_ASSERT_V_SAME(TYPE);                                                 \
    // Apply the specified 'META_FUNC' to the specified 'TYPE' and confirm that
    // the result value and the specified 'result' are the same.  Also confirm
    // that the result value of the 'META_FUNC' and the value of the
    // 'META_FUNC_v' variable are the same.

#define TYPE_ASSERT_REF(META_FUNC, TYPE, result)                              \
    ASSERT(result == META_FUNC<TYPE&>::value);                                \
    TYPE_ASSERT_V_SAME(TYPE&);                                                \
    // Apply the specified 'META_FUNC' to the specified 'TYPE' reference and
    // confirm that the result value and the specified 'result' are the same.
    // Also confirm that the result value of the 'META_FUNC' and the value of
    // the 'METU_FUNC_v' variable are the same.

#define TYPE_ASSERT(META_FUNC, TYPE, result)                              \
    TYPE_ASSERT_PREFIX(META_FUNC, TYPE, result);                          \
    // Apply the specified 'META_FUNC' to the specified 'TYPE' and confirm that
    // the result value and the specified 'result' are the same.


struct NoAllocateMember {
    // A class that does not meet the allocator requirements.  No 'allocate'.
    typedef int value_type;
};

struct NoValueType {
    // A class that does not meet the allocator requirements.  No 'value_type'.
    int *allocate(size_t);
};

struct AllocateTwoParams {
    // A class that does not meet the allocator requirements.  The 'allocate'
    // method is not callable with a single 'size_t' parameter.
    typedef int value_type;
    int *allocate(size_t, void *);
        // a function signature that does not match the allocator requirements.
};

struct AllocateBadParam {
    // A class that does not meet the allocator requirements.  The 'allocate'
    // method is not callable with a single 'size_t' parameter.
    typedef int value_type;
    int *allocate(const StructTestType &);
        // a function signature that does not match the allocator requirements.
};


//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

// In this section we show intended use of this component.
//
///Example 1: Verify if a class meets the requirements for an allocator.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types meet the requirements
// for allocators.
//
// First, we create a struct type 'MyAllocator':
//..
    struct MyAllocator
    {
        typedef int value_type;
        int *allocate(size_t);
            // Allocate some memory for use by the caller.
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

    switch (test) { case 0:  // Zero is always the leading case.
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

// Now, we instantiate the 'bsl::IsStdAllocator' template for both a type that
// does not meet the allocator requirements and the defined type 'MyClass',
// that does, asserting the 'value' static data member of each instantiation.
//..
    ASSERT(false == bsl::IsStdAllocator<int>::value);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    ASSERT(true  == bsl::IsStdAllocator<MyAllocator>::value);
#else
    ASSERT(false == bsl::IsStdAllocator<MyAllocator>::value);
#endif
//..
// Note that if the current compiler supports the variable the templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::IsStdAllocator_v' variable as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(false == bsl::IsStdAllocator_v<int>);
    ASSERT(true  == bsl::IsStdAllocator_v<MyAllocator>);
#endif
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::IsStdAllocator::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::IsStdAllocator' instantiations having various
        //   (template parameter) 'TYPE's has the correct value.
        //
        // Concerns:
        //: 1 'IsStdAllocator::value' is 'false' when 'TYPE' is a primitive
        //:   type.
        //:
        //: 2 'IsStdAllocator::value' is 'false' when 'TYPE' is an 'enum' type.
        //:
        //: 3 'IsStdAllocator::value' is 'false' when 'TYPE' is a 'class',
        //:   'struct', or 'union' type.
        //:
        //: 4 'IsStdAllocator::value' is 'false' when 'TYPE' is a pointer or
        //:   pointer-to-member type.
        //:
        //: 5 'IsStdAllocator::value' is 'false' when 'TYPE' is a function
        //:   type.
        //:
        //: 6 'IsStdAllocator::value' is 'true' when 'TYPE' is a  type that
        //:   meets the requirements of an allocator.
        //:
        //: 7 'IsStdAllocator::value' is 'false' when 'TYPE' is a type that
        //:   does not meet the requirements of an allocator.
        //:
        //: 8 That 'IsStdAllocator_v' equals to 'IsStdAllocator::value' for
        //:   a variety of template parameter types.
        //
        // Plan:
        //: 1 Verify that 'bsl::IsStdAllocator::value' has the correct value
        //:   for each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::IsStdAllocator::value
        //   bsl::IsStdAllocator_v
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::IsStdAllocator::value'\n"
                            "============================\n");

        // C-1,8
        TYPE_ASSERT_V(bsl::IsStdAllocator, void, false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, int,  false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, int,  false);

        // C-2,8
        TYPE_ASSERT_V(bsl::IsStdAllocator, EnumTestType, false);

        // C-3,8
        TYPE_ASSERT_V  (bsl::IsStdAllocator, StructTestType,       false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, StructTestType,       false);
        TYPE_ASSERT_V  (bsl::IsStdAllocator, UnionTestType,        false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, UnionTestType,        false);
        TYPE_ASSERT_V  (bsl::IsStdAllocator, Incomplete,           false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, Incomplete,           false);
        TYPE_ASSERT_V  (bsl::IsStdAllocator, BaseClassTestType,    false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, BaseClassTestType,    false);
        TYPE_ASSERT_V  (bsl::IsStdAllocator, DerivedClassTestType, false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, DerivedClassTestType, false);

        // C-4,8
        TYPE_ASSERT_V(bsl::IsStdAllocator, int*,                       false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, StructTestType*,            false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, int StructTestType::*,      false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, int StructTestType::* *,    false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, UnionTestType*,             false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, PMD BaseClassTestType::*,   false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, BaseClassTestType*,         false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, DerivedClassTestType*,      false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, Incomplete*,                false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, MethodPtrTestType,          false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, FunctionPtrTestType,        false);

        TYPE_ASSERT_REF(bsl::IsStdAllocator, int*,                      false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, StructTestType*,           false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, int StructTestType::*,     false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, int StructTestType::* *,   false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, UnionTestType*,            false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, PMD BaseClassTestType::*,  false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, PMD BaseClassTestType::* *,
                                                                        false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, BaseClassTestType*,        false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, DerivedClassTestType*,     false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, Incomplete*,               false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, MethodPtrTestType,         false);
        TYPE_ASSERT_REF(bsl::IsStdAllocator, FunctionPtrTestType,       false);

        // C-5,8
        TYPE_ASSERT_V(bsl::IsStdAllocator, int  (int),  false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, void (void), false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, int  (void), false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, void (int),  false);

        // C-6,8
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        TYPE_ASSERT_V(bsl::IsStdAllocator, MyAllocator,       true);
#else
        TYPE_ASSERT_V(bsl::IsStdAllocator, MyAllocator,       false);
#endif
        TYPE_ASSERT_V(bsl::IsStdAllocator, std::allocator<int>,        true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, std::allocator<float>,      true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, std::allocator<int *>,      true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, std::allocator<Incomplete>, true);


        TYPE_ASSERT_V(bsl::IsStdAllocator, bsl::allocator<int>,          true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, bsl::allocator<float>,        true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, bsl::allocator<int *>,        true);
        TYPE_ASSERT_V(bsl::IsStdAllocator, bsl::allocator<Incomplete>,   true);

        // C-7,8
        TYPE_ASSERT_V(bsl::IsStdAllocator, NoAllocateMember,  false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, NoValueType,       false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, AllocateTwoParams, false);
        TYPE_ASSERT_V(bsl::IsStdAllocator, AllocateBadParam,  false);

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
// Copyright 2021 Bloomberg Finance L.P.
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
