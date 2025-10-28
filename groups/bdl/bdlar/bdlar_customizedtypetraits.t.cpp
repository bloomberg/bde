// bdlar_customizedtypetraits.t.cpp                                   -*-C++-*-
#include <bdlar_customizedtypetraits.h>

#include <bdlar_accessorref.h>
#include <bdlar_manipulatorref.h>

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
#include <bsl_string.h>

#include <stdlib.h>  // `atoi`

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

namespace test {

                           // =======================
                           // struct CustomizedString
                           // =======================

struct CustomizedString {
    // PUBLIC DATA
    bsl::string d_value;
};

// MANIPULATORS
int bdlat_customizedTypeConvertFromBaseType(CustomizedString   *object,
                                            const bsl::string&  value)
{
    object->d_value = value;
    return 0;
}

// ACCESSORS
const bsl::string& bdlat_customizedTypeConvertToBaseType(
                                                const CustomizedString& object)
{
    return object.d_value;
}

}  // close namespace test

namespace BloombergLP {
namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<test::CustomizedString> : bsl::true_type {
};

template <>
struct BaseType<test::CustomizedString> {
    typedef bsl::string Type;
};

}  // close namespace bdlat_CustomizedTypeFunctions
}  // close enterprise namespace

namespace test {

struct Accessor {
    const bsl::string& baseValue;

    int operator()(const bsl::string& s) const
    {
        return s == baseValue;
    }
    template <class t_TYPE>
    int operator()(const t_TYPE& ) const
    {
        ASSERT(false);
        return false;
    }
};

struct BaseManipulator {
    bsl::string newValue;

    int operator()(bsl::string *s) const
    {
        *s = newValue;
        return 0;
    }
    template <class t_TYPE>
    int operator()(t_TYPE *) const
    {
        ASSERT(false);
        return -1;
    }
};

bool isEqual(bdlar::AnyConstRefData any, const bsl::string& baseValue)
{
    ASSERT(any.d_category == bdlat_TypeCategory::e_SIMPLE_CATEGORY);
    bdlar::SimpleTypeConstRef ref(any.d_object_p, any.d_vtable.d_simple_p);
    Accessor accessor = {baseValue};
    return ref.access(accessor);
}

}  // close namespace test

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

        using namespace test;
        BSLMF_ASSERT(bdlar::IsCustomized<CustomizedString>::value);

        CustomizedString str;

        bdlar::CustomizedTypeRef ref(str);
        ASSERT(ref.baseCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY);

        const bsl::string baseValue("abc");
        ASSERT(ref.convertFromBaseType(baseValue) == 0);
        ASSERT(str.d_value == baseValue);

        ASSERT(isEqual(ref.convertToBaseType(), baseValue));

        {
            bdlar::CustomizedTypeConstRef cref(ref);
            ASSERT(cref.baseCategory()==bdlat_TypeCategory::e_SIMPLE_CATEGORY);
            ASSERT(isEqual(cref.convertToBaseType(), baseValue));
        }

        {
            bdlar::CustomizedTypeConstRef cref(
                                    const_cast<const CustomizedString &>(str));
            ASSERT(cref.baseCategory()==bdlat_TypeCategory::e_SIMPLE_CATEGORY);
            ASSERT(isEqual(cref.convertToBaseType(), baseValue));
        }

        // Test `bdlat_CustomizedTypeFunctions` customization-points
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_CustomizedTypeFunctions::
                           IsCustomizedType<bdlar::CustomizedTypeRef>::value));

            typedef  bdlat_CustomizedTypeFunctions::BaseType<
                                      bdlar::CustomizedTypeRef>::Type BaseType;
            const BaseType& base =
                         bdlat_CustomizedTypeFunctions::convertToBaseType(ref);
            ASSERT(bdlat_CustomizedTypeFunctions::convertFromBaseType(
                                                        &ref, baseValue) == 0);
            (void) base;

            const bsl::string newBaseValue("newBaseValue");
            BaseManipulator baseManipulator = { newBaseValue };
            int rc = bdlat_customizedTypeCreateBaseAndConvert(&ref,
                                                              baseManipulator);
            ASSERT(rc == 0);
            ASSERT(str.d_value == newBaseValue);
            str.d_value = baseValue;
        }
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_CustomizedTypeFunctions::
                      IsCustomizedType<bdlar::CustomizedTypeConstRef>::value));

            bdlar::CustomizedTypeConstRef cref(ref);
            typedef  bdlat_CustomizedTypeFunctions::BaseType<
                                 bdlar::CustomizedTypeConstRef>::Type BaseType;
            const BaseType& base =
                        bdlat_CustomizedTypeFunctions::convertToBaseType(cref);
            (void) base;
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
