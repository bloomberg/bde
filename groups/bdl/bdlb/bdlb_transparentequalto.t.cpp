// bdlb_transparentequalto.t.cpp                                      -*-C++-*-
#include <bdlb_transparentequalto.h>

#include <bdlb_transparenthash.h>

#include <bslalg_constructorproxy.h>
#include <bslalg_typetraits.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'
#include <bsl_cstring.h>  // 'bsl::strcmp'
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// 'bdlb::TransparentEqualTo' provides a stateless type and thus very little to
// test.  The primary concern is that function call operator compares different
// types correctly.  CREATORS can be tested only for mechanical functioning.
// And BSL traits presence should be checked as we declare that
// 'bdlb::TransparentEqualTo' is an empty POD.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// [ 3] operator()(const LHS& lhs, const RHS& rhs) const
// [ 2] TransparentEqualTo()
// [ 2] TransparentEqualTo(const bdlb::TransparentEqualTo&)
// [ 2] ~TransparentEqualTo()
// [ 2] TransparentEqualTo& operator=(const bdlb::TransparentEqualTo&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
// [ 4] TESTING TYPEDEF
// [ 5] QoI: Support for empty base optimization

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
//                STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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
//                    NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::TransparentEqualTo Obj;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

template<class LHS, class RHS>
void testCompareInt()
    // Verify the correctness of the function call operator for variables of
    // the (template parameters) 'LHS' and 'RHS' integer types.
{
    int MIN_VALUE = -10;
    int MAX_VALUE =  10;
    for (int i = MIN_VALUE; i < MAX_VALUE; ++i) {
        for (int j = MIN_VALUE; j < MAX_VALUE; ++j) {
            const LHS  LEFT     = i;
            const RHS  RIGHT    = j;
            const bool EXPECTED = (i == j);

            // XLC version 16.1 (on AIX) complains about creating a const
            // object that does not have an "initializer or user-defined
            // default constructor".  To work around that, we create a
            // non-const object, and a const reference to that object, and use
            // the reference.
            Obj comp;
            const Obj& compare = comp;

            ASSERTV(i, j, EXPECTED == compare(LEFT, RIGHT));
        }
    }
}

template<class LHS, class RHS>
void testCompareString()
    // Verify the correctness of the function call operator for variables of
    // the (template parameters) 'LHS' and 'RHS' string types.
{
    static const struct {
        int         d_lineNum;  // source line number
        const char *d_str_p;    // specification string
    } DATA[] = {
        //line  string
        //----  ------------------------------------------------------------
        { L_,   "",                                                          },
        { L_,   "A",                                                         },
        { L_,   "B",                                                         },
        { L_,   "AB",                                                        },
        { L_,   "BC",                                                        },
        { L_,   "BCA",                                                       },
        { L_,   "CAB",                                                       },
        { L_,   "CDAB",                                                      },
        { L_,   "DABC"                                                       },
        { L_,   "ABCDE"                                                      },
        { L_,   "EDCBA"                                                      },
        { L_,   "ABCDEA"                                                     },
        { L_,   "ABCDEAB"                                                    },
        { L_,   "BACDEABC"                                                   },
        { L_,   "CBADEABCD"                                                  },
        { L_,   "CBADEABCDAB"                                                },
        { L_,   "CBADEABCDABC"                                               },
        { L_,   "CBADEABCDABCDE"                                             },
        { L_,   "CBADEABCDABCDEA"                                            },
        { L_,   "CBADEABCDABCDEAB"                                           },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA"                            },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEABCBADEABCDABCDEABCBADEABABC" }
    };

    bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    bslma::TestAllocator ta("test");

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        for (bsl::size_t j = 0; j < NUM_DATA; ++j) {
            const char                    *LEFT_SPEC  = DATA[i].d_str_p;
            const char                    *RIGHT_SPEC = DATA[j].d_str_p;
            bslalg::ConstructorProxy<LHS>  leftProxy( LEFT_SPEC,  &ta);
            bslalg::ConstructorProxy<RHS>  rightProxy(RIGHT_SPEC, &ta);
            const LHS&                     LEFT       = leftProxy.object();
            const RHS&                     RIGHT      = rightProxy.object();
            const bool                     EXPECTED   = (0 == bsl::strcmp(
                                                                  LEFT_SPEC,
                                                                  RIGHT_SPEC));

            // XLC version 16.1 (on AIX) complains about creating a const
            // object that does not have an "initializer or user-defined
            // default constructor".  To work around that, we create a
            // non-const object, and a const reference to that object, and use
            // the reference.
            Obj comp;
            const Obj& compare = comp;

            ASSERTV(i, j, EXPECTED == compare(LEFT, RIGHT));
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 6: {
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
///Example 1: Basic Use of 'bdlb::TransparentEqualTo'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique 'bsl::string' objects.
// 'bsl::unordered_set' is designed exactly for this purpose.  But imagine that
// we want to use 'bsl::string_view' objects for search operations within our
// container.  'bsl::unordered_set' uses 'bsl::equal_to' as default comparator.
// The problem is that even though the equality operator between 'bsl::string'
// and 'bsl::string_view' exists, the compiler will try to convert
// the 'bsl::string_view' objects to 'bsl::string' since 'bsl::equal_to' is
// parameterized by 'bsl::string'.  The compilation will fail because there is
// no such implicit conversion.  Even if the compilation were to succeed
// (because an implicit conversion were available), such an operation can lead
// to additional memory allocation for temporary objects.  The following code
// illustrates how to use 'bdlb::TransparentEqualTo' as a comparator for the
// standard container 'unordered_set', in this case to allow a
// 'bsl::unordered_set<bsl::string>' to be searched with a 'bsl::string_view'.
//
// First, we create a container that uses 'bdlb::TransparentEqualTo' as a
// comparator.  Note that to avoid implicit conversions we also have to use
// transparent hash functor:
//..
    typedef bsl::unordered_set<bsl::string,
                               bdlb::TransparentHash,
                               bdlb::TransparentEqualTo>  TransparentSet;

    TransparentSet  transparentSet;
//..
// Now, we fill the container with the strings:
//..
    transparentSet.insert("NY");
    transparentSet.insert("LA");
//..
// Finally, we observe that the container allows to use 'bsl::string_view'
// objects as a key and does not make any implicit conversions:
//..
    bsl::string_view newYork     ("NY");
    bsl::string_view losAngeles  ("LA");
    bsl::string_view sanFrancisco("SF");

    ASSERT(transparentSet.end() != transparentSet.find(newYork     ));
    ASSERT(transparentSet.end() != transparentSet.find(losAngeles  ));
    ASSERT(transparentSet.end() == transparentSet.find(sanFrancisco));
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING QOI: 'TransparentEqualTo' IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 Class 'bdlb::TransparentEqualTo' does not increase the size of an
        //:   object when used as a base class.
        //:
        //: 2 Object of 'bdlb::TransparentEqualTo' class increases size of an
        //:   object when used as a class member.
        //
        // Plan:
        //: 1 Define two identical non-empty classes with no padding, but
        //:   derive one of them from 'bdlb::TransparentEqualTo', then assert
        //:   that both classes have the same size. (C-1)
        //:
        //: 2 Create a non-empty class with an 'bdlb::TransparentEqualTo'
        //:   additional data member, assert that class size is larger than sum
        //:   of other data member's sizes. (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) cout
               << endl
               << "TESTING QOI: 'TransparentEqualTo' IS AN EMPTY TYPE" << endl
               << "==================================================" << endl;

        struct TwoInts {
            int d_a;
            int d_b;
        };

        struct DerivedInts : bdlb::TransparentEqualTo {
            int d_a;
            int d_b;
        };

        struct IntWithMember {
            Obj d_dummy;
            int d_a;
        };

        ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));
        ASSERT(sizeof(int)     <  sizeof(IntWithMember));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEF
        //   Comparator's transparency is determined by the presence of the
        //   'is_transparent' type.  We need to verify that the class offers
        //   the required typedef.
        //
        // Concerns:
        //: 1 The type 'is_transparent' is defined, publicly accessible and an
        //:   alias for 'void'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bsl::is_same'. (C-1)
        //
        // Testing:
        //  TESTING TYPEDEF
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TYPEDEF" << endl
                          << "===============" << endl;

        ASSERT((bsl::is_same<void, Obj::is_transparent>::value));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //
        // Concerns:
        //: 1 Objects of type 'bdlb::TransparentEqualTo' can be invoked as a
        //:   binary predicate returning 'bool' and taking two references
        //:   providing non-modifiable access.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the  first supplied argument is less than the second.
        //:
        //: 4 No memory is allocated from the default allocator.
        //
        // Plan:
        //: 1 Specify a set of integer types 'SI'.  For each pair 'si1' and
        //:   'si2' chosen from 'SI' create two variables of these types.
        //:   Execute inner loops that iterate over several values and compare
        //:   variables using function call operator.  Verify the result of
        //:   comparison.
        //:
        //: 2 Specify a set of string types 'SS'.  For each pair 'ss1' and
        //:   'ss2' chosen from 'SS' create two variables of these types.
        //:   For each variable iterate through a set of table values and
        //:   compare  variables using function call operator.  Verify the
        //:   result of comparison.  (C-1..3)
        //:
        //: 3 Verify that no memory have been allocated from the default
        //:   allocator.  (C-4)
        //
        // Testing:
        //   operator()(const LHS& lhs, const RHS& rhs) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FUNCTION CALL OPERATOR" << endl
                          << "======================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\tTesting operator with integer types." << endl;

        testCompareInt<int,      int     >();
        testCompareInt<int,      long int>();
        testCompareInt<long int, int     >();
        testCompareInt<long int, long int>();

        if (verbose) cout << "\tTesting operator with string types." << endl;

        testCompareString<bsl::string,       bsl::string      >();
        testCompareString<bsl::string,       bsl::string_view >();
        testCompareString<bsl::string_view , bsl::string      >();
        testCompareString<bsl::string_view , bsl::string_view >();

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile.
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
        //: 7 No memory is allocated by the default allocator.
        //
        // Plan:
        //: 1 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const bdlb::TransparentEqualTo'
        //:   object. (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bdlb::TransparentEqualTo' from the first. (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bdlb::TransparentEqualTo' object to leave scope and be
        //:   destroyed. (C-6)
        //:
        //: 6 Verify that no memory have been allocated from the default
        //:   allocator.  (C-7)
        //
        // Testing:
        //   TransparentEqualTo()
        //   TransparentEqualTo(const bdlb::TransparentEqualTo&)
        //   ~TransparentEqualTo()
        //   TransparentEqualTo& operator=(const bdlb::TransparentEqualTo&)
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
            const bdlb::TransparentEqualTo obj1 = bdlb::TransparentEqualTo();


            if (verbose) cout << "Copy initialization" << endl;
            bdlb::TransparentEqualTo obj2 = obj1;

            if (verbose) cout << "Copy assignment" << endl;
            obj2 = obj1;
            obj2 = obj2 = obj1;
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

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
        //: 1 Execute function call operator to verify functionality for simple
        //:   case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bdlb::TransparentEqualTo compare;

        ASSERT( compare(bsl::string("A"),      bsl::string_view("A")));
        ASSERT(!compare(bsl::string_view("A"), bsl::string("Z")     ));
        ASSERT( compare(bsl::string("z"),      bsl::string_view("z")));
        ASSERT(!compare(bsl::string_view("z"), bsl::string("a")     ));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
