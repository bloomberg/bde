// bdlar_dynamictyperef.t.cpp                                         -*-C++-*-
#include <bdlar_dynamictyperef.h>

#include <bdlar_anycastutil.h>

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
#include <bsl_vector.h>

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

                           // ===================
                           // class MyDynamicType
                           // ===================

class MyDynamicType {
  public:
    // PUBLIC DATA
    //union { // cannot contain `string` in C++03
        bsl::string      d_stringValue;
        bsl::vector<int> d_arrayValue;
    //};
    int d_selectionId;

    // CREATORS
    MyDynamicType() : d_selectionId(0) {}

    // MANIPULATORS
    void setString(const bsl::string &s)
    {
        d_selectionId = 0;
        d_stringValue = s;
    }
    void setArray(const bsl::vector<int> &v)
    {
        d_selectionId = 1;
        d_arrayValue = v;
    }

    // ACCESSORS
    bool isString() const { return d_selectionId == 0; }
    bool isArray() const { return d_selectionId == 1; }
};

}  // close namespace test

namespace BloombergLP {

template <>
struct bdlat_TypeCategoryDeclareDynamic<test::MyDynamicType> : bsl::true_type {
};

}  // close enterprise namespace

namespace test {

bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const MyDynamicType& object)
{
    switch(object.d_selectionId)
    {
      case 0: return bdlat_TypeCategory::e_SIMPLE_CATEGORY;           // RETURN
      case 1: return bdlat_TypeCategory::e_ARRAY_CATEGORY;            // RETURN
    }
    ASSERT(false); // should not be reached
    return bdlat_TypeCategory::e_DYNAMIC_CATEGORY;
}

template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateSimple(MyDynamicType  *object,
                                       t_MANIPULATOR&  manipulator)
{
    if (object->isString()) {
        return manipulator(&object->d_stringValue,
                           bdlat_TypeCategory::Simple());             // RETURN
    }
    return manipulator(object, bslmf::Nil());
}

template <class t_MANIPULATOR>
int bdlat_typeCategoryManipulateArray(MyDynamicType  *object,
                                      t_MANIPULATOR&  manipulator)
{
    if (object->isArray()) {
        return manipulator(&object->d_arrayValue,
                           bdlat_TypeCategory::Array());              // RETURN
    }
    return manipulator(object, bslmf::Nil());
}

template <class t_ACCESSOR>
int bdlat_typeCategoryAccessSimple(const MyDynamicType& object,
                                   t_ACCESSOR&          accessor)
{
    if (object.isString()) {
        return accessor(object.d_stringValue,
                        bdlat_TypeCategory::Simple());                // RETURN
    }
    return accessor(object, bslmf::Nil());
}

template <class t_ACCESSOR>
int bdlat_typeCategoryAccessArray(const MyDynamicType& object,
                                  t_ACCESSOR&          accessor)
{
    if (object.isArray()) {
        return accessor(object.d_arrayValue,
                        bdlat_TypeCategory::Array());                 // RETURN
    }
    return accessor(object, bslmf::Nil());
}

struct Accessor {
    const bsl::string& value;

    int operator()(const bsl::string& s) const
    {
        ASSERT(s == value);
        return 0;
    }
    template <class t_TYPE>
    int operator()(const t_TYPE& ) const
    {
        ASSERT(false);
        return -1;
    }
};

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
    int operator()(const bdlar::DynamicTypeConstRef&, bslmf::Nil) const
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
    int operator()(bdlar::DynamicTypeRef *, bslmf::Nil) const
    {
        return 0;
    }
};

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
        BSLMF_ASSERT(bdlar::IsDynamic<MyDynamicType>::value);
        BSLMF_ASSERT(bdlar::IsDynamic<bdlar::DynamicTypeRef>::value);
        BSLMF_ASSERT(bdlar::IsDynamic<bdlar::DynamicTypeConstRef>::value);

        MyDynamicType dynType;
        bdlar::DynamicTypeRef ref(dynType);
        ASSERT(ref.objectAddress() == &dynType);
        ASSERT(ref.typeCategory() == bdlat_TypeCategory::e_SIMPLE_CATEGORY);
        const bsl::string stringValue("abc");
        Accessor accessor = {stringValue};
        dynType.d_stringValue = stringValue;

        {
            bdlar::AnyConstRef any = ref.valueConstRef();
            ASSERT(any.typeCategory() ==
                                        bdlat_TypeCategory::e_SIMPLE_CATEGORY);
            bdlar::SimpleTypeConstRef value =
                                 bdlar::AnyCastUtil::toSimpleTypeConstRef(any);
            value.access(accessor);
        }
        {
            bdlar::AnyRef any = ref.valueRef();
            ASSERT(any.typeCategory() ==
                                        bdlat_TypeCategory::e_SIMPLE_CATEGORY);
            bdlar::SimpleTypeRef value =
                                      bdlar::AnyCastUtil::toSimpleTypeRef(any);
            value.access(accessor);
        }

        {
            bdlar::DynamicTypeConstRef cref(ref);
            ASSERT(cref.objectAddress() == &dynType);

            {
                bdlar::AnyConstRef any = cref.valueConstRef();
                ASSERT(any.typeCategory() ==
                                        bdlat_TypeCategory::e_SIMPLE_CATEGORY);
                bdlar::SimpleTypeConstRef value =
                                 bdlar::AnyCastUtil::toSimpleTypeConstRef(any);
                value.access(accessor);
            }
        }

        {
            bdlar::DynamicTypeConstRef cref(
                                   const_cast<const MyDynamicType &>(dynType));
            ASSERT(cref.objectAddress() == &dynType);
        }

        // Test `bdlat` customization-points
        {
            BSLMF_ASSERT(bdlar::IsDynamic<bdlar::DynamicTypeRef>::value);

            bdlat_ValueTypeFunctions::reset(&ref);
            UniversalManipulator manipulator;
            bdlat_TypeCategoryUtil::manipulateByCategory(&ref, manipulator);
            bdlat_TypeCategoryFunctions::select(ref);
            UniversalAccessor accessor;
            bdlat_TypeCategoryUtil::accessByCategory(ref, accessor);
        }
        {
            BSLMF_ASSERT(bdlar::IsDynamic<bdlar::DynamicTypeConstRef>::value);

            bdlar::DynamicTypeConstRef cref(ref);
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
