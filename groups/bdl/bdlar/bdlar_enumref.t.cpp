// bdlar_enumref.t.cpp                                                -*-C++-*-
#include <bdlar_enumref.h>

#include <bdlb_string.h>

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
#include <bsl_type_traits.h>

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

enum MyEnum {
    EnumFallbackValue = 0,
    EnumValue1 = 1,
    EnumValue2 = 2,
    EnumValue3 = 3
};

inline
int bdlat_enumFromInt(MyEnum *result, int number)
{
    if(number < 0 || number > 3) {
        return -1;                                                    // RETURN
    }
    *result = static_cast<MyEnum>(number);
    return 0;
}

inline
int bdlat_enumFromString(MyEnum *result, const char *string, int stringLength)
{
    if (bdlb::String::areEqualCaseless("EnumValue1",
                                       string,
                                       stringLength)) {
        *result = EnumValue1;
        return 0;                                                     // RETURN
    }

    if (bdlb::String::areEqualCaseless("EnumValue2",
                                       string,
                                       stringLength)) {
        *result = EnumValue2;
        return 0;                                                     // RETURN
    }

    if (bdlb::String::areEqualCaseless("EnumValue3",
                                       string,
                                       stringLength)) {
        *result = EnumValue3;
        return 0;                                                     // RETURN
    }

    if (bdlb::String::areEqualCaseless("EnumFallbackValue",
                                       string,
                                       stringLength)) {
        *result = EnumFallbackValue;
        return 0;                                                     // RETURN
    }

    return -1;
}

inline
void bdlat_enumToInt(int *result, const MyEnum& value)
{
    *result = static_cast<int>(value);
}

inline
void bdlat_enumToString(bsl::string *result, const MyEnum& value)
{
    static const char * const values[] = {
        "EnumFallbackValue",
        "EnumValue1",
        "EnumValue2",
        "EnumValue3"
    };

    int intValue;
    bdlat_enumToInt(&intValue, value);

    if(0 <= intValue && intValue <= 3) {
        *result = values[intValue];
    }
    else {
        *result = values[0];
    }
}

inline
bool bdlat_enumHasFallback(const MyEnum& )
{
    return true;
}

inline
bool bdlat_enumIsFallback(const MyEnum& value)
{
    return value == EnumFallbackValue;
}

inline
int bdlat_enumMakeFallback(MyEnum *result)
{
    *result = EnumFallbackValue;
    return 0;
}

}  // close namespace test

namespace BloombergLP {
namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<test::MyEnum> : bsl::true_type {
};

template <>
struct HasFallbackEnumerator<test::MyEnum> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

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
        BSLMF_ASSERT(bdlar::IsEnum<MyEnum>::value);
        BSLMF_ASSERT(::BloombergLP::bdlat_EnumFunctions::
                                         HasFallbackEnumerator<MyEnum>::value);

        MyEnum enumeration = EnumValue1;

        bdlar::EnumRef ref(enumeration);

        int intValue;
        ASSERT(ref.hasFallback());
        ref.toInt(&intValue);
        ASSERT(intValue == static_cast<int>(enumeration));

        ASSERT(ref.fromInt(3) == 0);
        ASSERT(enumeration == EnumValue3);
        ref.toInt(&intValue);
        ASSERT(intValue == static_cast<int>(enumeration));

        bsl::string stringValue;
        ref.toString(&stringValue);
        ASSERTV(stringValue == "EnumValue3");

        ASSERT(ref.fromString("EnumValue2", 10) == 0);
        ASSERT(enumeration == EnumValue2);
        ref.toString(&stringValue);
        ASSERT(stringValue == "EnumValue2");

        ASSERT(!ref.isFallback());
        ASSERT(ref.makeFallback() == 0);
        ASSERT(ref.isFallback());
        ASSERT(enumeration == EnumFallbackValue);

        {
            bdlar::EnumConstRef cref(ref);
            cref.toInt(&intValue);
            ASSERT(intValue == static_cast<int>(enumeration));
        }

        {
            bdlar::EnumConstRef cref(const_cast<const MyEnum &>(enumeration));
            cref.toInt(&intValue);
            ASSERT(intValue == static_cast<int>(enumeration));
        }

        // Test `bdlat_EnumFunctions` customization-points
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_EnumFunctions::
                                        IsEnumeration<bdlar::EnumRef>::value));
            BSLMF_ASSERT((::BloombergLP::bdlat_EnumFunctions::
                                HasFallbackEnumerator<bdlar::EnumRef>::value));

            bdlat_ValueTypeFunctions::reset(&ref);
            bdlat_EnumFunctions::makeFallback(&ref);
            bdlat_EnumFunctions::fromInt(&ref, 1);
            bdlat_EnumFunctions::fromString(&ref, "EnumValue1", 10);
            bdlat_EnumFunctions::hasFallback(ref);
            bdlat_EnumFunctions::isFallback(ref);
            int intValue;
            bdlat_EnumFunctions::toInt(&intValue, ref);
            bsl::string strValue;
            bdlat_EnumFunctions::toString(&strValue, ref);
        }
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_EnumFunctions::
                                   IsEnumeration<bdlar::EnumConstRef>::value));
            BSLMF_ASSERT((::BloombergLP::bdlat_EnumFunctions::
                           HasFallbackEnumerator<bdlar::EnumConstRef>::value));

            bdlar::EnumConstRef cref(ref);
            bdlat_EnumFunctions::hasFallback(cref);
            bdlat_EnumFunctions::isFallback(cref);
            int intValue;
            bdlat_EnumFunctions::toInt(&intValue, cref);
            bsl::string strValue;
            bdlat_EnumFunctions::toString(&strValue, cref);
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
