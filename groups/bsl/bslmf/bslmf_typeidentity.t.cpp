// bslmf_typeidentity.t.cpp                                           -*-C++-*-

#include <bslmf_typeidentity.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#include <type_traits> // 'std::type_identity', 'std::type_identity_t'
#endif

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#endif

using std::printf;
using std::fprintf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component is a metafunction that takes a type argument and returns a
// type that is the same as that argument.  Testing consists of invoking the
// metafunction with different kinds of arguments and verifying that the
// returned type is the same.  There are two abbreviated forms of the
// metafunction that are tested the same way.  When 'std::type_identity' and
// 'std::type_identify_t' are available from the native library,
// 'bsl::type_identity' and 'bsl::type_identity_t' must be aliased to those
// standard meta functions.
//
// ----------------------------------------------------------------------------
// [ 1] FULL TEST
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLES
// [ 2] CONCERN: Aliased to standard types when available.
// ----------------------------------------------------------------------------

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

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct TestClass { };
enum TestEnum { };
template <class T1, class T2> struct TestTmplt;  // No body needed

template <class TYPE>
struct FullTest {
    // Full test of this component.

    static void test(int line)
        // Test 'bsl::type_identity', 'bsl::type_identity_t', and
        // 'BSLMF_TYPEIDENTITY_T', when applied to 'TYPE'.  Note that the
        // 'BSLMF_TYPEIDENTITY_T' macro is valid only when called on a
        // dependent type, and thus must be tested within a function template
        // or member function of a class template.
    {
        ASSERTV(line, (bsl::is_same<typename bsl::type_identity<TYPE>::type,
                                    TYPE>::value));
    #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        ASSERTV(line, (bsl::is_same<bsl::type_identity_t<TYPE>, TYPE>::value));
    #endif
        ASSERTV(line, (bsl::is_same<BSLMF_TYPEIDENTITY_T(TYPE), TYPE>::value));
    }
};

template <class T1, class T2>
struct FullTest<TestTmplt<T1, T2> > {
    // Full test of this component, specialized for 'TestTmplt<T1, T2>'.  The
    // reason for this specialization is to ensure that the
    // 'BSLMF_TYPEIDENTITY_T' macro works correctly even if its argument
    // contains a comma.

    static void test(int line)
        // Test 'bsl::type_identity', 'bsl::type_identity_t', and
        // 'BSLMF_TYPEIDENTITY_T', when applied to 'TestTmplt<T1, T2>'.  Note
        // that the 'BSLMF_TYPEIDENTITY_T' macro is valid only when called on a
        // dependent type, and thus must be tested within a function template
        // or class template.
    {
        typedef TestTmplt<T1, T2> Exp;

        ASSERTV(line,
                (bsl::is_same<typename
                              bsl::type_identity<TestTmplt<T1, T2> >::type,
                              Exp>::value));
    #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        ASSERTV(line, (bsl::is_same<bsl::type_identity_t<TestTmplt<T1, T2> >,
                                    Exp>::value));
    #endif
        ASSERTV(line, (bsl::is_same<BSLMF_TYPEIDENTITY_T(TestTmplt<T1, T2>),
                                    Exp>::value));
    }
};

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: Requiring explicit function template arguments
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A function template can often deduce the types of its arguments, but
// sometimes we wish to prevent such deduction and require the user to supply
// the desired type explicitly.  In this example, we'll declare a cast
// function, 'implicitCast', that is invoked 'implicitCast<T>(arg)'.  The goal
// is cast the 'arg' to type 'T', but only if 'arg' is implicitly convertible
// to 'T'.
//
// First, we'll define a type 'TestType', that is implicitly convertible from
// 'int' but only explicitly convertible from 'const char *':
//..
    struct TestType {
        TestType(int) { }                                           // IMPLICIT
        explicit TestType(const char*) { }
    };
//..
// Next, we'll define 'implicitCastNAIVE', a naive and insufficient attempt at
// defining 'implicitCast':
//..
    template <class TYPE>
    TYPE implicitCastNAIVE(TYPE arg)
    {
        return arg;
    }
//..
// Next, we try to use 'implicitCastNAIVE'.  The first invocation below
// correctly casts an 'int' to 'TestType'.  The second invocation should, and
// does, fail to compile because 'const char*' is not implicitly convertible to
// 'TestType'.  In the third invocation, we forgot the '<TestType>' template
// parameter.  Surprisingly (for the user), the code compiles anyway because
// 'implicitCastNAIVE' *deduced* 'TYPE' to be 'const char*' and returns its
// argument unmodified, i.e., doing no casting whatsoever:
//..
    TestType v1(implicitCastNAIVE<TestType>(5));      // OK
//! TestType v2(implicitCastNAIVE<TestType>("bye"));  // Fails correctly.
    TestType v3(implicitCastNAIVE("hello"));          // Succeeds incorrectly.
//..
// Now, we implement 'implicitCast' correctly, using 'bsl::type_identity' to
// prevent implicit template-argument deduction:
//..
    template <class TYPE>
    TYPE implicitCast(typename bsl::type_identity<TYPE>::type arg)
    {
        return arg;
    }
//..
// Finally, we try using 'implicitCast' both correctly and incorrectly.  As
// before, the first invocation below correctly casts an 'int' to 'TestType'
// and second invocation correctly fails to compile.  Unlike the
// 'implicitCastNAIVE' example, however, the third invocation correctly fails
// to compile because 'TYPE' is not deducable for a parameter of type
// 'bsl::type_identity<TYPE>::type'.
//..
    TestType v4(implicitCast<TestType>(5));      // OK
//! TestType v5(implicitCast<TestType>("bye"));  // Fails correctly.
//! TestType v6(implicitCast("hello"));          // Fails correctly.
//..
// Note that 'typename bsl::type_identity<TYPE>::type' can be replaced by the
// more concise 'bsl::type_identity_t<TYPE>' (compatible with C++11 and later)
// or 'BSLMF_TYPEIDENTITY_T(TYPE)' (compatible with all C++ versions).
//
///Example 2: preventing ambiguous argument deduction in function templates
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we illunstrate how to prevent ambiguities when type
// deductions occurs on multiple function-template arguments.  Our sample
// function returns a number within a range two-thirds of the way between the
// start and end of the range.  The types of the arguments determine the type
// of the result.
//
// First, we implement the function using a simple but potentially ambiguous
// interface:
//..
    template <class NUMTYPE>
    inline NUMTYPE twoThirdsOfTheWay1(NUMTYPE first, NUMTYPE last)
    {
        return first + (last - first) * 2 / 3;
    }
//..
// Now, try to invoke our function.  We get into trouble when the two arguments
// have different types; the compiler is unable to deduce a single 'NUMTYPE':
//..
    int    i1 = twoThirdsOfTheWay1(0, 6);     // OK, 'NUMTYPE' is 'int'
//! double d1 = twoThirdsOfTheWay1(0, 0.75);  // Ambiguous: 'int' vs 'double'
//..
// Next, we try again, this time using 'bsl::type_identity' to suppress type
// deduction on the first argument.  The *first* argument, rather than the
// *second* argument is chosen for this treatment because the first argument of
// a numeric range is so often 0, which happens to be an 'int' but is often
// used, without losing precision, with 'unsigned', 'float', and 'double'
// values.  The second argument, conversely, usually carries a significant
// value whose type is important:
//..
    template <class NUMTYPE>
    inline NUMTYPE twoThirdsOfTheWay(BSLMF_TYPEIDENTITY_T(NUMTYPE) first,
                                     NUMTYPE                       last)
    {
        return first + (last - first) * 2 / 3;
    }

    int    i2 = twoThirdsOfTheWay(0, 6);     // OK, 'NUMTYPE' is 'int'
    double d2 = twoThirdsOfTheWay(0, 0.75);  // OK, 'NUMTYPE' is 'double'
//..
// Finally, we verify that our 'twoThirdsOfTheWay' function worked correctly:
//..
    void usageExample2()
    {
        ASSERT(4 == i2);
        ASSERT(0.5 == d2);
        ASSERT(0 == twoThirdsOfTheWay(4U, -2));
    }
//..

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        // usageExample1();  // Example 1 has no "main", hence nothing to call
        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIASED TO STANDARD META FUNCTIONS
        //
        // Concerns:
        //: 1 The meta functions 'bsl::type_identity' and
        //:   'bsl::type_identity_v' should be aliased to their standard
        //:   library analogs when the latter is available from the native
        //:   library.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY' is
        //:   defined, use 'bsl::is_same' to compare the two meta functions
        //:   using a representative type.
        //
        // Testing:
        //   CONCERN: Aliased to standard types when available.
        // --------------------------------------------------------------------

        if (verbose) printf("\nALIASED TO STANDARD META FUNCTIONS"
                            "\n==================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        typedef int T;

        if (veryVerbose) printf("\nTesting 'type_identity' using 'int'.\n");

        ASSERT((bsl::is_same<bsl::type_identity<T>,
                             std::type_identity<T> >::value));

        if (veryVerbose) printf("\nTesting 'type_identity_t' using 'int'.\n");

        ASSERT((bsl::is_same<bsl::type_identity_t<T>,
                             std::type_identity_t<T> >::value));
#else
        if (veryVerbose) printf(
                            "\nSkipped: neither standard type is available\n");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 'bsl::type_identity<TYPE>::type' is the same as 'TYPE'.
        //: 2 'bsl::type_identity_t<TYPE>' is the same as 'TYPE' (C++11 +)
        //: 3 'BSLMF_TYPEIDENTITY_T(TYPE)' is the same as 'TYPE'.
        //: 4 Concerns 1-3 apply to integer types, pointer types, class types,
        //:   enum types, and instantiations of class templates (including when
        //:   the class template has multiple parameters).
        //
        // Plan:
        //: 1 Verify that 'bsl::is_same<bsl::type_identity<int>::type,
        //:   int>::value' is 'true'. (C-1)
        //: 2 Verify that 'bsl::is_same<bsl::type_identity_t<int>, int>::value'
        //:   is true.  (C-2)
        //: 3 Verify that 'bsl::is_same<BSLMF_TYPEIDENTITY_T(int), int>::value'
        //:   is true.  (C-3)
        //: 4 Repeat steps 1-3, substituting a class type, enum type, and class
        //:   template instantiation for 'int'.
        //
        // Testing:
        //      FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        FullTest<int>::test(L_);
        FullTest<TestClass>::test(L_);
        FullTest<TestEnum>::test(L_);
        FullTest<TestTmplt<char, bool> >::test(L_);

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
