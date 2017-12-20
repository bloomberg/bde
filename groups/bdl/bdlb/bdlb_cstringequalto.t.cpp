// bdlb_cstringequalto.t.cpp                                          -*-C++-*-
#include <bdlb_cstringequalto.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::CStringEqualTo' provides a stateless type and thus very little to
// test.  The primary concern is that function call operator compares C-strings
// correctly.  CREATORS can be tested only for mechanical functioning.  And BSL
// traits presence should be checked as we declare that 'bdlb::CStringEqualTo'
// is an empty POD.
//
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 2] CStringEqualTo()
// [ 2] CStringEqualTo(const CStringEqualTo&)
// [ 2] ~CStringEqualTo()
// [ 2] CStringEqualTo& operator=(const CStringEqualTo&)
// [ 3] operator()(const char *, const char *) const
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 4] Standard typedefs
// [ 5] BSL Type Traits
// [ 6] QoI: Support for empty base optimization

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------
typedef bdlb::CStringEqualTo Obj;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bsl::is_trivially_copyable<Obj>::value);
BSLMF_ASSERT(bsl::is_trivially_default_constructible<Obj>::value);

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)         veryVerbose;
    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
// Example 1: Basic Use of 'bdlb::CStringEqualTo'
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'bdlb::CStringEqualTo' object as a binary predicate for the standard library
// function 'bsl::equal' to test that two ranges of null-terminated character
// strings are equal.
//
// First, we create few sequences with null-terminated character strings,
// making sure that their elements have different memory addresses:
//..
    const char hello1[] = { 'h', 'e', 'l', 'l', 'o', 0};
    const char hello2[] = { 'h', 'e', 'l', 'l', 'o', 0};

    const char* arrayA[3] = { "A", "B", hello1 };
    const char* arrayB[3] = { "A", "B", hello2 };
//..
// Now, use bdlb::CStringEqualTo() as a binary predicate to compare sequences:
//..
    bool bdlbEqualTo = bsl::equal(arrayA, arrayA+3, arrayB,
                                  bdlb::CStringEqualTo());
    bool bslEqualTo  = bsl::equal(arrayA, arrayA+3, arrayB,
                                  bsl::equal_to<const char *>());
//..
// Finally, we observe that 'bdlb::CStringEqualTo' compares character string by
// their values, while default comparator compares addresses:
//..
    ASSERT( true  == bdlbEqualTo );
    ASSERT( false == bslEqualTo );
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING QoI: 'CStringEqualTo' IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 class 'bdlb::CStringEqualTo' does not increase the size of an
        //:   object when used as a base class.
        //:
        //: 2 Object of 'bdlb::CStringEqualTo' class increases size of an
        //:   object when used as a class member.
        //
        // Plan:
        //: 1 Define two identical non-empty classes with no padding, but
        //:   derive one of them from 'bdlb::CStringLess', then assert that
        //:   both classes have the same size. (C-1)
        //:
        //: 2 Create a non-empty class with an 'bdlb::CStringLess' additional
        //:   data member, assert that class size is larger than sum of other
        //:   data member's sizes. (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) cout
                   << endl
                   << "TESTING QoI: 'CStringEqualTo' IS AN EMPTY TYPE" << endl
                   << "==============================================" << endl;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedTwoInts : Obj {
            int a;
            int b;
        };

        struct TwoIntsWithMember {
            int a;
            int b;
            Obj dummy;
        };

        ASSERT(sizeof(TwoInts) == sizeof(DerivedTwoInts));
        ASSERT(sizeof(TwoInts)  < sizeof(TwoIntsWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BSL TYPE TRAITS
        //   The functor is an empty POD, and should have the appropriate BSL
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is trivially copyable.
        //:
        //: 2 The class is trivially default constructable.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type. (C-1..2)
        //
        // Testing:
        //   BSL Type Traits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BSL TYPE TRAITS" << endl
                          << "=======================" << endl;

        ASSERT((bsl::is_trivially_copyable<Obj>::value));
        ASSERT((bsl::is_trivially_default_constructible<Obj>::value));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function.
        //
        // Concerns:
        //: 1 The typedef 'first_argument_type' is publicly accessible and an
        //:   alias for 'const char *'.
        //:
        //: 2 The typedef 'second_argument_type' is publicly accessible and an
        //:   alias for 'const char *'.
        //:
        //: 3 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bool'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bsl::is_same'. (C-1..3)
        //
        // Testing:
        //   Standard typedefs
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING STANDARD TYPEDEFS" << endl
                          << "=========================" << endl;

        ASSERT((bsl::is_same<const char *, Obj::first_argument_type>::value));
        ASSERT((bsl::is_same<const char *, Obj::second_argument_type>::value));
        ASSERT((bsl::is_same<bool,         Obj::result_type>::value));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //
        // Concerns:
        //: 1 Objects of type 'bdlb::CStringEqualTo' can be invokes as a binary
        //:   predicate returning 'bool' and taking two 'const char *'
        //:   arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments have the same string value.
        //:
        //: 4 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of C-strings for
        //:   comparison, and a flag value indicating whether one string is
        //:   equal to the other.
        //:
        //: 2 For each row 'R' in the table of P-1 verify that the function
        //:   call operator, when invoked on C-string values from 'R', returns
        //:   the expected value.  (C-1..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //:
        //: 4 Verify that no memory have been allocated from the default
        //:   allocator.  (C-5)
        //
        // Testing:
        //   operator()(const char *, const char *) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FUNCTION CALL OPERATOR" << endl
                          << "======================" << endl;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct {
            int         d_line;
            const char *d_lhs;
            const char *d_rhs;
            bool        d_expected;
        } DATA[] = {
            // LINE    LHS     RHS     EXPECTED
            // ----    ---     ---     --------
            {  L_,     "",     "",     true    },
            {  L_,     "",     "\0",   true    },
            {  L_,     "",     "A",    false   },
            {  L_,     "A",    "A",    true    },
            {  L_,     "A",    "a",    false   },
            {  L_,     "A",    "AA",   false   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const Obj compare = Obj();

        for (int i = 0; i != NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *LHS      = DATA[i].d_lhs;
            const char *RHS      = DATA[i].d_rhs;
            const bool  EXPECTED = DATA[i].d_expected;

            ASSERTV(LINE, compare(LHS, RHS) == EXPECTED);
            ASSERTV(LINE, compare(RHS, LHS) == EXPECTED);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(compare(0, "Hello world"));
            ASSERT_SAFE_FAIL(compare("Hello world", 0));
            ASSERT_SAFE_FAIL(compare(0, 0));
            ASSERT_SAFE_PASS(compare("Hello", "world"));
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that all
        //   expected expressions compile.
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //:
        //: 2 Objects can be created using the copy constructor.
        //:
        //: 3 The copy constructor is not declared as explicit.
        //:
        //: 4 Objects can be assigned to from constant objects.
        //:
        //: 5 Assignments operations can be chained.
        //:
        //: 6 Objects can be destroyed.
        //:
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 2 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const CStringEqualTo'
        //:   object. (C-1)
        //:
        //: 3 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'CStringEqualTo' from the first. (C-2,3)
        //:
        //: 4 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 5 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 6 Verify the destructor is publicly accessible by allowing the
        //:   'CStringEqualTo' object(s) to leave scope and be destroyed. (C-6)
        //:
        //: 7 Verify that no memory have been allocated from the default
        //:   allocator. (C-7)
        //
        // Testing:
        //   CStringEqualTo()
        //   CStringEqualTo(const CStringEqualTo&)
        //   ~CStringEqualTo()
        //   CStringEqualTo& operator=(const CStringEqualTo&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "IMPLICITLY DEFINED OPERATIONS" << endl
                          << "=============================" << endl;


        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            if (verbose) cout << "Value initialization" << endl;
            const Obj obj1 = Obj();


            if (verbose) cout << "Copy initialization" << endl;
            Obj obj2 = obj1;

            if (verbose) cout << "Copy assignment" << endl;
            obj2 = obj1;
            obj2 = obj2 = obj1;
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'compare' using the default constructor. (C-1)
        //:
        //: 2 Call the 'compare' functor with two string literals. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj compare;

        const char stringA1[] = { 'A', 0};
        const char stringA2[] = { 'A', 0};

        ASSERT(compare(stringA1, "A"));
        ASSERT(compare(stringA1, stringA2));
        ASSERT(!compare(stringA1, "a"));
        ASSERT(!compare("A", "Z"));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
