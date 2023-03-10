// bdlb_pairutil.t.cpp                                                -*-C++-*-
#include <bdlb_pairutil.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsltf_templatetestfacility.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_type_traits.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component is a utility that provides a single static function template,
// 'tie'.  The primary concern is to ensure that this function template
// satisfies its contract.
//
// Note that as long as 'tie' always returns the correct type, there are no
// allocator issues involved, because reference types are not allocator-aware.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] template<T1, T2> bsl::pair<T1&, T2&> tie(T1&, T2&)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

//=============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
bool     verbose = false;
bool veryVerbose = false;
}  // close unnamed namespace

//=============================================================================
//                           CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace bdlb_pairutil_test_case_2 {

typedef int    Scalar;
typedef int    Array[2];
typedef int    Function(int);

class Class {
  public:
    Class() {}

  private:
    Class(const Class&);
    Class& operator=(const Class&);
};

union Union {
    int    d_int;
    double d_double;
};

// Alias some types that will be used as macro arguments.  (We cannot wrap
// type-ids in parentheses as we can expressions.)
typedef pair<char, short> PairCharShort;
typedef pair<int,  long>  PairIntLong;

#define BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST \
    Scalar,        const Scalar,            \
    Array,         const Array,             \
    Class,         const Class,             \
    Union,         const Union,             \
    Function,                               \
    PairCharShort, const PairCharShort,     \
    PairIntLong,   const PairIntLong

int const0(int)
{
    return 0;
}

int id(int x)
{
    return x;
}

template <class t_TYPE>
t_TYPE& getFirst()
    // Return an lvalue of type 't_TYPE' that is distinct from the one returned
    // by 'getSecond<t_TYPE>'.  This template is specialized for various types
    // for which the default definition might not do the right thing.
{
    static t_TYPE x;
    return x;
}

template <class t_TYPE>
t_TYPE& getSecond()
    // Return an lvalue of type 't_TYPE' that is distinct from the one returned
    // by 'getFirst<t_TYPE>'.  This template is specialized for various types
    // for which the default definition might not do the right thing.
{
    static t_TYPE y;
    return y;
}

template <>
Function& getFirst()
{
    return const0;
}

template <>
Function& getSecond()
{
    return id;
}

template <class t_TYPE1, class t_TYPE2>
void assertSameType(const t_TYPE1&, const t_TYPE2&)
    // Statically assert that the template parameters 't_TYPE1' and 't_TYPE2'
    // are the same, which ensures that the argument types are almost the same,
    // other than that they might differ in const-qualification.  Note that in
    // C++03, we have no way to distinguish whether the original arguments were
    // lvalues or rvalues, but surely we won't accidentally change the return
    // type of of 'bdlb::PairUtil::tie' to a reference (or a 'const'
    // non-reference) (I hope).
{
    BSLMF_ASSERT((bsl::is_same<t_TYPE1, t_TYPE2>::value));
}

template <class t_TYPE>
void assertDistinctIfSameUnqualType(const t_TYPE *first, const t_TYPE *second)
    // Assert that the specified 'first' and 'second' pointers are unequal.
    // Note that this overload is chosen only if the arguments point to the
    // same type (up to cv-qualification).
{
    ASSERT(first != second);
}

void assertDistinctIfSameUnqualType(...) {}

template <class t_FIRST>
struct TestWithFirst {
    template <class t_SECOND>
    struct TestWithSecond {
        static void run() {
            t_FIRST&  first =
                         getFirst<typename bsl::remove_const<t_FIRST>::type>();
            t_SECOND& second =
                       getSecond<typename bsl::remove_const<t_SECOND>::type>();

            assertDistinctIfSameUnqualType(&first, &second);

            pair<t_FIRST&, t_SECOND&> expected(first, second);
            pair<t_FIRST&, t_SECOND&> result = bdlb::PairUtil::tie(first,
                                                                   second);
            assertSameType(expected, bdlb::PairUtil::tie(first, second));
            ASSERT(&first  == &result.first);
            ASSERT(&second == &result.second);
        }
    };

    static void run()
    {
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                          TestWithSecond,
                                          run,
                                          BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST);
    }
};

void run()
    // Run test case 2 with all ordered pairs of scalar, array, class, union,
    // and function types, as well as two specializations of 'bsl::pair' (used
    // in order to ensure that 'tie' does not call an unintended constructor,
    // since 'bsl::pair' has many constructors.)
{
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                          TestWithFirst,
                                          run,
                                          BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST);
}
#undef BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST
}  // close namespace bdlb_pairutil_test_case_2

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
// Suppose we need to implement a function that takes a 'bsl::map' and stores
// into out-parameters the key and value corresponding to the first entry in
// the map.  Using 'bsl::map's container interface, we can obtain a reference
// to a 'bsl::pair' of the key and value.  We can then use
// 'bdlb::PairUtil::tie' to assign from both the key and value in a single
// expression:
bool getFirst(int                              *key,
              bsl::string                      *value,
              const bsl::map<int, bsl::string>& map)
    // Load into the specified 'key' and the specified 'value' the key and
    // value for the first entry in the specified 'map' and return 'true', or
    // else fail by storing 0 and an empty string and return 'false' when 'map'
    // is empty.
{
    if (map.empty()) {
        *key = 0;
        value->clear();
        return false;                                                 // RETURN
    }
    bdlb::PairUtil::tie(*key, *value) = *map.begin();
    return true;
}

void usageExample()
    // Run the usage example defined in the component header.
{
    bsl::map<int, bsl::string> map;
    map[30782530] = "bbi10";

    int         uuid;
    bsl::string username;

    bool result = getFirst(&uuid, &username, map);
    ASSERT(result);
    ASSERT(30782530 == uuid);
    ASSERT("bbi10"  == username);
}
}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? atoi(argv[1]) : 0;
        verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Copy the usage example from the component header, change 'assert'
        //:   to 'ASSERT', and run the function 'usageExample'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE" "\n"
                          << "=====================" "\n";
        usageExample();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'tie'
        //
        // Concerns:
        //: 1 The 'tie' function can accept lvalues of scalar, array, class,
        //:   union, and function types.
        //:
        //: 2 The 'tie' function can accept both const and non-const lvalues.
        //:
        //: 3 The return type of the 'tie' function is always a 'bsl::pair'
        //:   whose first type is an lvalue reference to the first argument's
        //:   type and whose second type is an lvalue reference to the second
        //:   argument's type.
        //:
        //: 4 The addresses of the first and second elements of the returned
        //:   pair from 'tie' (which are the addresses of their referents) are
        //:   equal to the addresses of the arguments.
        //:
        //: 5 The 'tie' function always calls the correct constructor of
        //:   'bsl::pair' so as to ensure that the 'first' (resp. 'second')
        //:   member of the result has the same address as the first (resp.
        //:   second) argument.
        //
        // Plan:
        //: 1 For all possible ordered pairs from a particular set of scalar,
        //:   array, class, union, and function type, and const-qualified
        //:   versions thereof, create one lvalue of each type.  For each such
        //:   combination, call the 'tie' function and verify that the result
        //:   has the expected type.  Then, verify that the 'first' member's
        //:   address is the same as the address of the first lvalue, and the
        //:   'second' member's address is the same as the address of the
        //:   second lvalue.  (C-1..4)
        //:
        //: 2 Repeat P-1 using some combinations of two 'bsl::pair'
        //:   specializations.  (C-5)
        //
        // Testing:
        //   template<T1, T2> bsl::pair<T1&, T2&> tie(T1&, T2&)
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING 'tie'\n"
               << "=============\n";

        bdlb_pairutil_test_case_2::run();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The component is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Verify that 'bdlb::PairUtil::tie' yields the correct result for a
        //:   simple assignment and a simple comparison.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        // Test assignment through 'tie'.
        const char            *a_str = "a";
        pair<int, const char*> p(1, a_str);

        int         first  = 0;
        const char *second = 0;

        bdlb::PairUtil::tie(first, second) = p;

        ASSERT(1     == first);
        ASSERT(a_str == second);

        // Test comparison of two objects returned by 'tie'.
        const int x1 = 2;
        const int y1 = 5;
        const int x2 = 3;
        const int y2 = 4;

        ASSERT(bdlb::PairUtil::tie(x1, y1) < bdlb::PairUtil::tie(x2, y2));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." "\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." "\n";
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
