// bdlar_sequenceref.t.cpp                                            -*-C++-*-
#include <bdlar_sequenceref.h>

#include <bdlar_accessorref.h>
#include <bdlar_manipulatorref.h>
#include <bdlar_arrayref.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_formattingmode.h>

#include <bsla_maybeunused.h>

#include <bdlb_string.h>

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

                           // =================
                           // struct MySequence
                           // =================

struct MySequence {
    // PUBLIC DATA
    int              d_field0;
    bsl::string      d_field1;
    bsl::vector<int> d_field2;

    // PUBLIC CLASS DATA
    static const bdlat_AttributeInfo ATTRIBUTE_INFO[];
};

const bdlat_AttributeInfo MySequence::ATTRIBUTE_INFO[] = {
    {
        0,
        "field0",
        sizeof("field0") - 1,
        "Field 0",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        1,
        "field1",
        sizeof("field1") - 1,
        "Field 1",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        2,
        "field2",
        sizeof("field2") - 1,
        "Field 2",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// MANIPULATORS

template <class t_MANIPULATOR>
int bdlat_sequenceManipulateAttribute(MySequence     *object,
                                      t_MANIPULATOR&  manipulator,
                                      int             attributeId)
{
    switch (attributeId) {
      case 0:
        return manipulator(&object->d_field0, MySequence::ATTRIBUTE_INFO[0]);
                                                                      // RETURN
      case 1:
        return manipulator(&object->d_field1, MySequence::ATTRIBUTE_INFO[1]);
                                                                      // RETURN
      case 2:
        return manipulator(&object->d_field2, MySequence::ATTRIBUTE_INFO[2]);
                                                                      // RETURN
      default:
        return -1; // not found                                       // RETURN
    }
}

template <class t_MANIPULATOR>
int bdlat_sequenceManipulateAttribute(MySequence     *object,
                                      t_MANIPULATOR&  manipulator,
                                      const char     *attributeName,
                                      int             attributeNameLength)
{
    if (bdlb::String::areEqualCaseless("field0",
                                       attributeName,
                                       attributeNameLength)) {
        return bdlat_sequenceManipulateAttribute(object, manipulator, 0);
                                                                      // RETURN
    }
    if (bdlb::String::areEqualCaseless("field1",
                                       attributeName,
                                       attributeNameLength)) {
        return bdlat_sequenceManipulateAttribute(object, manipulator, 1);
                                                                      // RETURN
    }
    if (bdlb::String::areEqualCaseless("field2",
                                        attributeName,
                                        attributeNameLength)) {
        return bdlat_sequenceManipulateAttribute(object, manipulator, 2);
                                                                      // RETURN
    }
    return -1; // not found
}

template <class t_MANIPULATOR>
int bdlat_sequenceManipulateAttributes(MySequence     *object,
                                       t_MANIPULATOR&  manipulator)
{
    int rc;
    rc = bdlat_sequenceManipulateAttribute(object, manipulator, 0);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }

    rc = bdlat_sequenceManipulateAttribute(object, manipulator, 1);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }

    rc = bdlat_sequenceManipulateAttribute(object, manipulator, 2);
    return rc;
}

// ACCESSORS

template <class t_ACCESSOR>
int bdlat_sequenceAccessAttribute(const MySequence& object,
                                  t_ACCESSOR&       accessor,
                                  int               attributeId)
{
    switch (attributeId) {
      case 0:
        return accessor(object.d_field0, MySequence::ATTRIBUTE_INFO[0]);
                                                                      // RETURN
      case 1:
        return accessor(object.d_field1, MySequence::ATTRIBUTE_INFO[1]);
                                                                      // RETURN
      case 2:
        return accessor(object.d_field2, MySequence::ATTRIBUTE_INFO[2]);
                                                                      // RETURN
      default:
        return -1; // not found                                       // RETURN
    }
}

template <class t_ACCESSOR>
int bdlat_sequenceAccessAttribute(const MySequence&  object,
                                  t_ACCESSOR&        accessor,
                                  const char        *attributeName,
                                  int                attributeNameLength)
{
    if (bdlb::String::areEqualCaseless("field0",
                                       attributeName,
                                       attributeNameLength)) {
        return bdlat_sequenceAccessAttribute(object, accessor, 0);    // RETURN
    }
    if (bdlb::String::areEqualCaseless("field1",
                                       attributeName,
                                       attributeNameLength)) {
        return bdlat_sequenceAccessAttribute(object, accessor, 1);    // RETURN
    }
    if (bdlb::String::areEqualCaseless("field2",
                                       attributeName,
                                       attributeNameLength)) {
        return bdlat_sequenceAccessAttribute(object, accessor, 2);    // RETURN
    }
    return -1; // not found
}

template <class t_ACCESSOR>
int bdlat_sequenceAccessAttributes(const MySequence& object,
                                   t_ACCESSOR&       accessor)
{
    int rc;
    rc = bdlat_sequenceAccessAttribute(object, accessor, 0);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }

    rc = bdlat_sequenceAccessAttribute(object, accessor, 1);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }

    rc = bdlat_sequenceAccessAttribute(object, accessor, 2);
    return rc;
}

bool bdlat_sequenceHasAttribute(const MySequence&  ,
                                const char        *attributeName,
                                int                attributeNameLength)
{
    return bdlb::String::areEqualCaseless("field0",
                                          attributeName,
                                          attributeNameLength)
        || bdlb::String::areEqualCaseless("field1",
                                          attributeName,
                                          attributeNameLength)
        || bdlb::String::areEqualCaseless("field2",
                                          attributeName,
                                          attributeNameLength);
}

bool bdlat_sequenceHasAttribute(const MySequence& , int attributeId)
{
    return 0 <= attributeId && attributeId <= 2;
}

}  // close namespace test

namespace BloombergLP {
namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<test::MySequence> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace

namespace test {

#define FIELD0_VALUE  6
#define FIELD1_VALUE  "text"
#define FIELD2_VALUE1 1
#define FIELD2_VALUE2 2

struct Accessor0 {
    template <class t_INFO>
    int operator()(const int& value, const t_INFO& info) const
    {
        ASSERT(info.id() == 0);
        ASSERT(value == FIELD0_VALUE);
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(const t_VALUE& , const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} accessor0;

struct Accessor1 {
    template <class t_INFO>
    int operator()(const bsl::string& value, const t_INFO& info) const
    {
        ASSERT(info.id() == 1);
        ASSERT(value == FIELD1_VALUE);
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(const t_VALUE& , const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} accessor1;

struct Accessor2 {
    template <class t_INFO>
    int operator()(const bdlar::ArrayConstRef& array, const t_INFO& info) const
    {
        ASSERT(info.id() == 2);
        ASSERT(array.size() == 2);
        ASSERT(array.elementCategory() ==
                                        bdlat_TypeCategory::e_SIMPLE_CATEGORY);
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(const t_VALUE& , const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} accessor2;

int universalAccessorCalls;

struct UniversalAccessor {
    template <class t_INFO>
    int operator()(const int& value, const t_INFO& info) const
    {
        ASSERT(info.id() == universalAccessorCalls);
        universalAccessorCalls++;
        switch(info.id())
        {
          case 0: return accessor0(value, info);                    // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_INFO>
    int operator()(const bsl::string& value, const t_INFO& info) const
    {
        ASSERT(info.id() == universalAccessorCalls);
        universalAccessorCalls++;
        switch(info.id())
        {
          case 1: return accessor1(value, info);                    // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_INFO>
    int operator()(const bdlar::ArrayConstRef& array, const t_INFO& info) const
    {
        ASSERT(info.id() == universalAccessorCalls);
        universalAccessorCalls++;
        switch(info.id())
        {
          case 2: return accessor2(array, info);                    // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_VALUE, class t_INFO>
    int operator()(const t_VALUE& , const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} universalAccessor;

struct Manipulator0 {
    template <class t_INFO>
    int operator()(int *value, const t_INFO& info) const
    {
        accessor0(*value, info);
        (*value)++;
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(t_VALUE *, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} manipulator0;

struct Manipulator1 {
    template <class t_INFO>
    int operator()(bsl::string *value, const t_INFO& info) const
    {
        accessor1(*value, info);
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(t_VALUE *, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} manipulator1;

struct Manipulator2 {
    template <class t_INFO>
    int operator()(bdlar::ArrayRef *value, const t_INFO& info) const
    {
        accessor2(bdlar::ArrayConstRef(*value), info);
        return 0;
    }
    template <class t_VALUE, class t_INFO>
    int operator()(t_VALUE *, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} manipulator2;

int universalManipulatorCalls;

struct UniversalManipulator {
    template <class t_INFO>
    int operator()(int *value, const t_INFO& info) const
    {
        ASSERT(info.id() == universalManipulatorCalls);
        universalManipulatorCalls++;
        switch(info.id())
        {
          case 0: return manipulator0(value, info);                   // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_INFO>
    int operator()(bsl::string *value, const t_INFO& info) const
    {
        ASSERT(info.id() == universalManipulatorCalls);
        universalManipulatorCalls++;
        switch(info.id())
        {
          case 1: return manipulator1(value, info);                   // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_INFO>
    int operator()(bdlar::ArrayRef *value, const t_INFO& info) const
    {
        ASSERT(info.id() == universalManipulatorCalls);
        universalManipulatorCalls++;
        switch(info.id())
        {
          case 2: return manipulator2(value, info);                   // RETURN
          default: ASSERT(false); // mustn't be reached
        }
        return 1; // error
    }

    template <class t_VALUE, class t_INFO>
    int operator()(t_VALUE *, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} universalManipulator;

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
        BSLMF_ASSERT(bdlar::IsSequence<MySequence>::value);

        MySequence seq;
        seq.d_field0 = FIELD0_VALUE;
        seq.d_field1 = FIELD1_VALUE;
        seq.d_field2.push_back(FIELD2_VALUE1);
        seq.d_field2.push_back(FIELD2_VALUE2);

        bdlar::SequenceRef ref(seq);
        ASSERT(ref.objectAddress() == &seq);

        ASSERT( ref.hasAttribute(0));
        ASSERT( ref.hasAttribute(1));
        ASSERT( ref.hasAttribute(2));
        ASSERT(!ref.hasAttribute(3));

        ASSERT( ref.hasAttribute("field0", 6));
        ASSERT( ref.hasAttribute("field1", 6));
        ASSERT( ref.hasAttribute("field2", 6));
        ASSERT(!ref.hasAttribute("field3", 6));

        ref.accessAttribute(accessor0, 0);
        ref.accessAttribute(accessor1, 1);
        ref.accessAttribute(accessor2, 2);

        ref.accessAttribute(accessor0, "field0", 6);
        ref.accessAttribute(accessor1, "field1", 6);
        ref.accessAttribute(accessor2, "field2", 6);

        universalAccessorCalls = 0;
        ref.accessAttributes(universalAccessor);
        ASSERT(universalAccessorCalls == 3);

        ref.manipulateAttribute(manipulator0, 0);
        ASSERT(seq.d_field0 == FIELD0_VALUE + 1);
        seq.d_field0 = FIELD0_VALUE;
        ref.manipulateAttribute(manipulator1, 1);
        ref.manipulateAttribute(manipulator2, 2);

        ref.manipulateAttribute(manipulator0, "field0", 6);
        ASSERT(seq.d_field0 == FIELD0_VALUE + 1);
        seq.d_field0 = FIELD0_VALUE;
        ref.manipulateAttribute(manipulator1, "field1", 6);
        ref.manipulateAttribute(manipulator2, "field2", 6);

        universalManipulatorCalls = 0;
        ref.manipulateAttributes(universalManipulator);
        ASSERT(universalManipulatorCalls == 3);
        ASSERT(seq.d_field0 == FIELD0_VALUE + 1);
        seq.d_field0 = FIELD0_VALUE;

        {
            bdlar::SequenceConstRef cref(ref);
            ASSERT(cref.objectAddress() == &seq);

            ASSERT( cref.hasAttribute(0));
            ASSERT( cref.hasAttribute(1));
            ASSERT( cref.hasAttribute(2));
            ASSERT(!cref.hasAttribute(3));

            ASSERT( cref.hasAttribute("field0", 6));
            ASSERT( cref.hasAttribute("field1", 6));
            ASSERT( cref.hasAttribute("field2", 6));
            ASSERT(!cref.hasAttribute("field3", 6));

            cref.accessAttribute(accessor0, 0);
            cref.accessAttribute(accessor1, 1);
            cref.accessAttribute(accessor2, 2);

            cref.accessAttribute(accessor0, "field0", 6);
            cref.accessAttribute(accessor1, "field1", 6);
            cref.accessAttribute(accessor2, "field2", 6);

            universalAccessorCalls = 0;
            ref.accessAttributes(universalAccessor);
            ASSERTV(universalAccessorCalls, universalAccessorCalls == 3);
        }

        {
            bdlar::SequenceConstRef cref(const_cast<const MySequence &>(seq));
            ASSERT(cref.objectAddress() == &seq);
        }

        // Test `bdlat_SequenceFunctions` customization-points
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_SequenceFunctions::
                                       IsSequence<bdlar::SequenceRef>::value));

            bdlat_ValueTypeFunctions::reset(&ref);
            seq.d_field0 = FIELD0_VALUE;
            seq.d_field1 = FIELD1_VALUE;
            seq.d_field2.push_back(FIELD2_VALUE1);
            seq.d_field2.push_back(FIELD2_VALUE2);

            bdlat_SequenceFunctions::manipulateAttribute(&ref, manipulator0,
                                                         0);
            seq.d_field0 = FIELD0_VALUE;
            bdlat_SequenceFunctions::manipulateAttribute(&ref, manipulator0,
                                                         "field0", 6);
            seq.d_field0 = FIELD0_VALUE;
            universalManipulatorCalls = 0;
            bdlat_SequenceFunctions::manipulateAttributes(
                                                         &ref,
                                                         universalManipulator);
            seq.d_field0 = FIELD0_VALUE;
            bdlat_SequenceFunctions::hasAttribute(ref, 0);
            bdlat_SequenceFunctions::hasAttribute(ref, "field0", 6);
            bdlat_SequenceFunctions::accessAttribute(ref, accessor0, 0);
            bdlat_SequenceFunctions::accessAttribute(ref, accessor0,
                                                     "field0", 6);
            universalAccessorCalls = 0;
            bdlat_SequenceFunctions::accessAttributes(ref, universalAccessor);
        }
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_SequenceFunctions::
                                  IsSequence<bdlar::SequenceConstRef>::value));

            bdlar::SequenceConstRef cref(ref);
            bdlat_SequenceFunctions::hasAttribute(cref, 0);
            bdlat_SequenceFunctions::hasAttribute(cref, "field0", 6);
            bdlat_SequenceFunctions::accessAttribute(cref, accessor0, 0);
            bdlat_SequenceFunctions::accessAttribute(cref, accessor0,
                                                     "field0", 6);
            universalAccessorCalls = 0;
            bdlat_SequenceFunctions::accessAttributes(cref, universalAccessor);
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
