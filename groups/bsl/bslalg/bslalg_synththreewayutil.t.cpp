// bslalg_synththreewayutil.t.cpp                                     -*-C++-*-
#include <bslalg_synththreewayutil.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <algorithm>  // 'lexicographical_compare_three_way'
#include <list>
#include <stdio.h>
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
typedef bslalg::SynthThreeWayUtil Obj;
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides utilities for 'operator<=>'
// implementation.
//-----------------------------------------------------------------------------
// [ 2] SynthThreeWayUtil::compare
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
namespace {

                           // =======================
                           // class TypeWithSpaceship
                           // =======================

struct TypeWithSpaceship {
    // Type with '<=>' operator.

    int d_value;

    auto operator<=>(const TypeWithSpaceship&) const = default;
};

                           // ======================
                           // class TypeWithLessOnly
                           // ======================

struct TypeWithLessOnly {
    // Type with '<' operator.

    int d_value;
};

bool operator<(const TypeWithLessOnly& lhs, const TypeWithLessOnly& rhs)
{
    return lhs.d_value < rhs.d_value;
}

                           // =============
                           // class Wrapped
                           // =============

template <class t_TYPE>
struct Wrapped {
    // Type with '<=>' operator defined in terms of comparisons defined for
    // 't_TYPE'.

    t_TYPE d_value;
};

template <class t_TYPE>
Obj::Result<t_TYPE> operator<=>(const Wrapped<t_TYPE>& lhs,
                                const Wrapped<t_TYPE>& rhs)
{
    return Obj::compare(lhs.d_value, rhs.d_value);
}

template <class t_TYPE, class t_COMPARISON_CATEGORY>
void testCase3()
    // Verify sanity of the defined comparison operators.
{
    Wrapped<t_TYPE> v1{1}, v2{2};

    ASSERTV((bslmf::IsSame<decltype(v1 <=> v2),
                           t_COMPARISON_CATEGORY>::value));

    ASSERT(v1 <=> v2 < 0);
    ASSERT(v2 <=> v1 > 0);

    ASSERT(v1 <=> v1 == 0);

    ASSERT(v1 <  v2);
    ASSERT(v1 <= v2);

    ASSERT(v2 >  v1);
    ASSERT(v2 >= v1);
}

// For USAGE EXAMPLE

template <class T, class A = std::allocator<T>>
class list {
    std::list<T,A> d_list;
public:
    auto begin() const { return d_list.begin(); }
    auto end() const { return d_list.end(); }
};

}  // close unnamed namespace

namespace bsl { using std::lexicographical_compare_three_way; }

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Example 1: Implementing '<=>' For a Sequence Container
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use 'bslalg::SynthThreeWayUtil' to implement the
// three-way comparison operator ('<=>') for a list container (whose details
// have been elided):
//..
template <class T, class A>
bslalg::SynthThreeWayUtil::Result<T> operator<=>(const list<T,A>& lhs,
                                                 const list<T,A>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                                       lhs.begin(),
                                       lhs.end(),
                                       rhs.begin(),
                                       rhs.end(),
                                       bslalg::SynthThreeWayUtil::compare);
}
//..

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

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
        //: 1 Incorporate the usage example from header into test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
        // Create some instance of the template
        list<int> list1, list2;

        // Empty lists are equal
        ASSERT(!(list1 <  list2));
        ASSERT(!(list1 >  list2));
        ASSERT(  list1 <= list2);
        ASSERT(  list1 >= list2);
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'SynthThreeWayUtil::compare'
        //
        // Concerns:
        //: 1 The function uses 'operator<=>' if available.
        //:
        //: 2 Otherwise the function uses 'operator<' if available.
        //
        // Plan:
        //: 1 Create a class template 'Wrapped' that holds a value of the
        //:   specified (wrapped) type and defines the '<=>' operator using the
        //:   wrapped value and 'SynthThreeWayUtil::compare'.
        //:
        //: 2 Create a type that wraps 'int' value and defines the '<=>'
        //:   operator - 'TypeWithSpaceship'.
        //:
        //: 3 Create a type that wraps 'int' value and defines the '<'
        //:   operator - 'TypeWithLessOnly'.
        //:
        //: 4 Verify that the comparison operators are defined correctly for
        //:   'Wrapped<TypeWithSpaceship>' and 'Wrapped<TypeLessOnly>'.
        //
        // Testing:
        //   SynthThreeWayUtil::compare
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'SynthThreeWayUtil::compare'"
                            "\n===================================\n");

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
        testCase3<TypeWithSpaceship, std::strong_ordering>();
        testCase3<TypeWithLessOnly, std::weak_ordering>();
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The provided utilities are sufficiently functional.
        //
        // Plan:
        //: 1 Write any code with 'bslalg::SynthThreeWayUtil' here.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
        Wrapped<int> v1{1}, v2{2};

        ASSERT(v1 <  v2);
        ASSERT(v1 <= v2);

        ASSERT(v2 >  v1);
        ASSERT(v2 >= v1);
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
