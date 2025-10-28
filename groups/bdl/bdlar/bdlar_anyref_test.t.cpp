// bdlar_anyref_test.t.cpp                                            -*-C++-*-

#include <bdlar_anyref_test.h>

#include <bdlar_anyref.h>
#include <bdlar_anymanipulators.h>

#include <bdlar_simpletyperef.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_iostream.h>

#include <stdlib.h>  // `atoi`

namespace BloombergLP {
namespace bdlar {

// Forward
class ArrayRef;
class ArrayConstRef;
class ChoiceRef;
class ChoiceConstRef;
class CustomizedTypeRef;
class CustomizedTypeConstRef;
class DynamicTypeRef;
class DynamicTypeConstRef;
class EnumRef;
class EnumConstRef;
class NullableValueRef;
class NullableValueConstRef;
class SequenceRef;
class SequenceConstRef;

}  // close package namespace
}  // close enterprise namespace

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                       CLASSES AND TYPES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

                           // ========================
                           // struct UniversalAccessor
                           // ========================

struct UniversalAccessor {
    int operator()(const bdlar::ArrayConstRef&,
                   bdlat_TypeCategory::Array) const
    {
        return 0;
    }
    int operator()(const bsl::vector<char>&,
                   bdlat_TypeCategory::Array) const
    {
        return 0;
    }
    int operator()(const bdlar::ChoiceConstRef&,
                   bdlat_TypeCategory::Choice) const
    {
        return 0;
    }
    int operator()(const bdlar::CustomizedTypeConstRef&,
                   bdlat_TypeCategory::CustomizedType) const
    {
        return 0;
    }
    int operator()(const bdlar::EnumConstRef&,
                   bdlat_TypeCategory::Enumeration) const
    {
        return 0;
    }
    int operator()(const bdlar::NullableValueConstRef&,
                   bdlat_TypeCategory::NullableValue) const
    {
        return 0;
    }
    int operator()(const bdlar::SequenceConstRef&,
                   bdlat_TypeCategory::Sequence) const
    {
        return 0;
    }
    template <class t_SIMPLE>
    int operator()(const t_SIMPLE&, bdlat_TypeCategory::Simple) const
    {
        return 0;
    }
    int operator()(const bdlar::AnyConstRef&, bslmf::Nil) const
    {
        return 0;
    }
};

                           // ===========================
                           // struct UniversalManipulator
                           // ===========================

struct UniversalManipulator {
    int operator()(bdlar::ArrayRef *, bdlat_TypeCategory::Array) const
    {
        return 0;
    }
    int operator()(bsl::vector<char> *, bdlat_TypeCategory::Array) const
    {
        return 0;
    }
    int operator()(bdlar::ChoiceRef *, bdlat_TypeCategory::Choice) const
    {
        return 0;
    }
    int operator()(bdlar::CustomizedTypeRef *,
                   bdlat_TypeCategory::CustomizedType) const
    {
        return 0;
    }
    int operator()(bdlar::EnumRef *, bdlat_TypeCategory::Enumeration) const
    {
        return 0;
    }
    int operator()(bdlar::NullableValueRef *,
                   bdlat_TypeCategory::NullableValue) const
    {
        return 0;
    }
    int operator()(bdlar::SequenceRef *, bdlat_TypeCategory::Sequence) const
    {
        return 0;
    }
    template <class t_SIMPLE>
    int operator()(t_SIMPLE *, bdlat_TypeCategory::Simple) const
    {
        return 0;
    }
    int operator()(bdlar::AnyRef *, bslmf::Nil) const
    {
        return 0;
    }
};

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Add any component-related code here.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        int value;
        bdlar::SimpleTypeRef simpleRef(value);
        bdlar::AnyRef ref(simpleRef.objectAddress(), simpleRef.vtable());

        // Test `bdlat` customization-points (dynamic type)
        {
            BSLMF_ASSERT(bdlar::IsDynamic<bdlar::AnyRef>::value);

            bdlat_ValueTypeFunctions::reset(&ref);
            UniversalManipulator manipulator;
            bdlat_TypeCategoryUtil::manipulateByCategory(&ref, manipulator);
            bdlat_TypeCategoryFunctions::select(ref);
            UniversalAccessor accessor;
            bdlat_TypeCategoryUtil::accessByCategory(ref, accessor);
        }
        {
            BSLMF_ASSERT(bdlar::IsDynamic<bdlar::AnyConstRef>::value);

            bdlar::AnyConstRef cref(ref);
            bdlat_TypeCategoryFunctions::select(cref);
            UniversalAccessor accessor;
            bdlat_TypeCategoryUtil::accessByCategory(cref, accessor);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
