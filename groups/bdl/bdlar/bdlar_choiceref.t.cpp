// bdlar_choiceref.t.cpp                                              -*-C++-*-
#include <bdlar_choiceref.h>

#include <bdlar_accessorref.h>
#include <bdlar_manipulatorref.h>

#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>

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

                           // ==============
                           // class MyChoice
                           // ==============

class MyChoice {
  public:
    // PUBLIC DATA
    //union { // cannot contain `string` in C++03
        int         d_selection0;
        bsl::string d_selection1;
    //};
    int d_selectionId;

    // CREATORS
    MyChoice() : d_selectionId(-1) {}

    // PUBLIC CLASS DATA
    static const bdlat_SelectionInfo SELECTION_INFO[];
};

const bdlat_SelectionInfo MyChoice::SELECTION_INFO[] = {
    {
        0,
        "selection0",
        sizeof("selection0") - 1,
        "Selection 0",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        1,
        "selection1",
        sizeof("selection1") - 1,
        "Selection 1",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// MANIPULATORS

int bdlat_choiceMakeSelection(MyChoice *object, int selectionId)
{
    switch (selectionId)
    {
      case 0: {
        object->d_selectionId = selectionId;
        object->d_selection0 = 0;
      } break;
      case 1: {
        object->d_selectionId = selectionId;
        object->d_selection1.clear();
      } break;
      default:
        return 1; // error                                            // RETURN
    }
    return 0;
}

int bdlat_choiceMakeSelection(MyChoice   *object,
                              const char *selectionName,
                              int         selectionNameLength)
{
    if (bdlb::String::areEqualCaseless("selection0",
                                       selectionName,
                                       selectionNameLength)) {
        return bdlat_choiceMakeSelection(object, 0);                  // RETURN
    }
    if (bdlb::String::areEqualCaseless("selection1",
                                       selectionName,
                                       selectionNameLength)) {
        return bdlat_choiceMakeSelection(object, 1);                  // RETURN
    }
    return 1; // not found
}

template <class t_MANIPULATOR>
int bdlat_choiceManipulateSelection(MyChoice       *object,
                                    t_MANIPULATOR&  manipulator)
{
    switch (object->d_selectionId)
    {
      case 0:
        return manipulator(&object->d_selection0, MyChoice::SELECTION_INFO[0]);
                                                                      // RETURN
      case 1:
        return manipulator(&object->d_selection1, MyChoice::SELECTION_INFO[1]);
                                                                      // RETURN
      default:
        ASSERT(false);
    }
    return 0;
}

// ACCESSORS
template <class t_ACCESSOR>
int bdlat_choiceAccessSelection(const MyChoice& object, t_ACCESSOR& accessor)
{
    switch (object.d_selectionId)
    {
      case 0:
        return accessor(object.d_selection0, MyChoice::SELECTION_INFO[0]);
                                                                      // RETURN
      case 1:
        return accessor(object.d_selection1, MyChoice::SELECTION_INFO[1]);
                                                                      // RETURN
      default:
        ASSERT(false);
    }
    return 0;
}

bool bdlat_choiceHasSelection(const MyChoice&  ,
                              const char      *selectionName,
                              int              selectionNameLength)
{
    return bdlb::String::areEqualCaseless("selection0",
                                          selectionName,
                                          selectionNameLength)
        || bdlb::String::areEqualCaseless("selection1",
                                          selectionName,
                                          selectionNameLength);
}

bool bdlat_choiceHasSelection(const MyChoice&, int selectionId)
{
    return 0 <= selectionId && selectionId <= 1;
}

int bdlat_choiceSelectionId(const MyChoice& object)
{
    return object.d_selectionId;
}

}  // close namespace test

namespace BloombergLP {
namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<test::MyChoice> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions
}  // close enterprise namespace

namespace test {

struct Accessor {
    template <class t_INFO>
    int operator()(const int&, const t_INFO& info) const
    {
        ASSERT(info.id() == 0);
        return 0;
    }
    template <class t_INFO>
    int operator()(const bsl::string&, const t_INFO& info) const
    {
        ASSERT(info.id() == 1);
        return 0;
    }
    template <class t_TYPE, class t_INFO>
    int operator()(const t_TYPE&, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} accessor;

struct Manipulator {
    template <class t_INFO>
    int operator()(int *value, const t_INFO& info) const
    {
        return accessor(*value, info);
    }
    template <class t_INFO>
    int operator()(bsl::string *value, const t_INFO& info) const
    {
        return accessor(*value, info);
    }
    template <class t_TYPE, class t_INFO>
    int operator()(t_TYPE *, const t_INFO& ) const
    {
        ASSERT(false);
        return -1;
    }
} manipulator;

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
        BSLMF_ASSERT(bdlar::IsChoice<MyChoice>::value);

        MyChoice choice;
        bdlar::ChoiceRef ref(choice);
        ASSERT(ref.objectAddress() == &choice);
        ASSERT(ref.selectionId() == -1);

        ASSERT( ref.hasSelection(0));
        ASSERT( ref.hasSelection(1));
        ASSERT(!ref.hasSelection(2));

        ASSERT( ref.hasSelection("selection0", 10));
        ASSERT( ref.hasSelection("selection1", 10));
        ASSERT(!ref.hasSelection("selection2", 10));

        ASSERT(ref.makeSelection(1) == 0);
        ASSERT(ref.selectionId() == 1);

        ASSERT(ref.makeSelection(0) == 0);
        ASSERT(ref.selectionId() == 0);

        ASSERT(ref.makeSelection("selection1", 10) == 0);
        ASSERT(ref.selectionId() == 1);

        ASSERT(ref.makeSelection("selection0", 10) == 0);
        ASSERT(ref.selectionId() == 0);

        ref.accessSelection(accessor);

        ref.manipulateSelection(manipulator);

        {
            bdlar::ChoiceConstRef cref(ref);
            ASSERT(cref.objectAddress() == &choice);

            ASSERT(ref.selectionId() == 0);

            ASSERT( ref.hasSelection(0));
            ASSERT( ref.hasSelection(1));
            ASSERT(!ref.hasSelection(2));

            ASSERT( ref.hasSelection("selection0", 10));
            ASSERT( ref.hasSelection("selection1", 10));
            ASSERT(!ref.hasSelection("selection2", 10));

            ref.accessSelection(accessor);
        }

        {
            bdlar::ChoiceConstRef cref(const_cast<const MyChoice &>(choice));
            ASSERT(cref.objectAddress() == &choice);
        }

        // Test `bdlat_ChoiceFunctions` customization-points
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_ChoiceFunctions::
                                           IsChoice<bdlar::ChoiceRef>::value));

            bdlat_ValueTypeFunctions::reset(&ref);
            bdlat_ChoiceFunctions::makeSelection(&ref, 0);
            bdlat_ChoiceFunctions::makeSelection(&ref, "selection0", 10);
            bdlat_ChoiceFunctions::manipulateSelection(&ref, manipulator);
            bdlat_ChoiceFunctions::selectionId(ref);
            bdlat_ChoiceFunctions::hasSelection(ref, 0);
            bdlat_ChoiceFunctions::hasSelection(ref, "selection0", 10);
            bdlat_ChoiceFunctions::accessSelection(ref, accessor);
        }
        {
            BSLMF_ASSERT((::BloombergLP::bdlat_ChoiceFunctions::
                                      IsChoice<bdlar::ChoiceConstRef>::value));

            bdlar::ChoiceConstRef cref(ref);
            bdlat_ChoiceFunctions::selectionId(cref);
            bdlat_ChoiceFunctions::hasSelection(cref, 0);
            bdlat_ChoiceFunctions::hasSelection(cref, "selection0", 10);
            bdlat_ChoiceFunctions::accessSelection(cref, accessor);
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
