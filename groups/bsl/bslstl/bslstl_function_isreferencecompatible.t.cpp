// bslstl_function_isreferencecompatible.t.cpp                        -*-C++-*-

#include <bslstl_function_isreferencecompatible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
// The metafunction under test is used to determine if, for template types
// 'FROM_TYPE' and 'TO_TYPE', a reference to the specified 'FROM_TYPE'
// parameter type can be substituted for a reference to the specified 'TO_TYPE'
// parameter type with no loss of information.  By default, this metafunction
// yields 'true_type' if 'FROM_TYPE' is the same as 'TO_TYPE'; else it yields
//'false_type'.  However, this template can be specialized to yield 'true_type'
// for other parameters that have compatible references.  The tests will verify
// the default behaviour of the metafunction under test, as well as the
// behaviour in the presence of a user provided specialization.  The tests will
// also check that the result is correct when one or both parameters is const.
//
// ----------------------------------------------------------------------------
//
// PUBLIC CLASS DATA
// [ 2] Function_IsReferenceCompatible
// [ 2] Function_IsReferenceCompatible user provided specialization
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                VERBOSITY
//-----------------------------------------------------------------------------

static int verbose             = 0;
static int veryVerbose         = 0;
static int veryVeryVerbose     = 0;
static int veryVeryVeryVerbose = 0;  // For test allocators

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

namespace bsl {

template <class PROTOTYPE>
class function {
    // Stub implementation of 'bsl::function' for testing.  An object of this
    // type is never created in this test driver, but conversion to and from
    // this type is tested.
};

}  // close namespace bsl

namespace BloombergLP {

template <class PROTOTYPE>
class bdef_Function;

template <class PROTOTYPE>
class bdef_Function<PROTOTYPE *> : public bsl::function<PROTOTYPE> {
    // Stub implementation of 'BloombergLP::bdef_Function' for testing.  An
    // object of this type is never created in this test driver, but conversion
    // to and from this type is tested.
};

namespace bslstl {

// Specialize 'bsl::Function_IsReferenceCompatible' metafunction to indicate
// that references to 'bdef_Function' can be used as references to
// 'bsl::function'.
template <class PROTOTYPE>
struct Function_IsReferenceCompatible<BloombergLP::bdef_Function<PROTOTYPE *>,
                                      bsl::function<PROTOTYPE> >
: bsl::true_type {
};

}  // close package namespace

}  // close enterprise namespace

struct BaseClass {
};
struct DerivedClass : BaseClass {
};

template <class FROM, class TO, bool EXP, int LINE>
void testIsReferenceCompatible()
    // Instantiates 'Function_IsReferenceCompatible<FROM, TO>' and verifies
    // that the result type has a const member 'value' such that
    // 'value == EXP'.  Also instantiate
    // 'Function_IsReferenceCompatible<FROM, TO>' with every combination of
    // const qualifier on 'FROM' and const qualifier on 'TO', verifying the
    // expected result 'value' (which is sometimes 'false' even if 'EXP' is
    // true).
{
    using namespace BloombergLP;

    const bool res = bslstl::Function_IsReferenceCompatible<FROM, TO>::value;
    LOOP_ASSERT(LINE, EXP == res);

#define TEST(TT, UU, EE)                                                      \
    LOOP_ASSERT(                                                              \
               LINE,                                                          \
               (EE == bslstl::Function_IsReferenceCompatible<TT, UU>::value))

    TEST(FROM, TO const, res);
    TEST(FROM const, TO const, res);
    TEST(FROM const, TO, false);

#undef TEST
}

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'Function_IsReferenceCompatible'
        //
        // Concerns:
        //: 1 An instantiation of 'Function_IsReferenceCompatible<T,U>' where
        //:   'T' and 'U' are the same type yields (class derived from)
        //:   'true_type'.
        //:
        //: 2 An *unspecialized* instantiation of
        //:   'Function_IsReferenceCompatible<T,U>' where 'T' and 'U' are
        //:   different types yields 'false_type', even if 'T' is derived from
        //:   'U'.
        //:
        //: 3 An instantiation of 'Function_IsReferenceCompatible<T,U>' that
        //:   matches an explicit specialization or partial specialization
        //:   yields the result of that specialization or partial
        //:   specialization.
        //:
        //: 4 If 'T' is const qualified and 'U' is not const qualified,
        //:   'Function_IsReferenceCompatible<T,U>' yields 'false_type' even
        //:   if 'T' and 'U' are reference compatible.  If 'U' is const
        //:   qualified, then the const qualifiers on both 'T' and 'U' are
        //:   ignored.  (This semantic adheres to the language rule that 'U'
        //:   type must be of same or greater cv qualification than 'T' type).
        //
        // Plan:
        //: 1 For concern 1, instantiate
        //:   'Function_IsReferenceCompatible<T,T>', where 'T' is 'int', a
        //:   'bsl::function' specialization and a 'bdef_Function'
        //:   specialization.  Verify that the contained 'value' constant is
        //:   'true'.
        //:
        //: 2 For concern 2, instantiate 'Function_IsReferenceCompatible<T,U>'
        //:   where 'T' and 'U' are different, incompatible types, including
        //:   the case where 'T' is derived from 'U'.  Verify that the
        //:   contained 'value' constant is 'false'.
        //:
        //: 3 For concern 3, instantiate 'Function_IsReferenceCompatible<T,U>'
        //:   where 'T' is a specialization of 'bdef_Function' and 'U' is a
        //:   specialization of 'bsl::function' based on the same function
        //:   prototype.  Verify that it yields 'true_type' (because there is
        //:   an explicit specialization).  Repeat this step using
        //:   'bdef_Function' and 'bsl::function' based on different function
        //:   prototypes and verify that it yields 'false_type'.
        //:
        //: 4 For concern 4, repeat the above steps with const qualifiers on
        //:   the first parameter, the second parameter, or both.  Verify that
        //:   if the first parameter is const and the second is not const, then
        //:   the result is 'false_type'; otherwise the result is the same as
        //:   without the const qualifiers.
        //
        // Testing:
        //      Function_IsReferenceCompatible
        //      Function_IsReferenceCompatible user provided specialization
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'Function_IsReferenceCompatible'"
                   "\n=======================================\n");

        typedef bsl::function<int(char *)>     bslFunc1;
        typedef bdef_Function<int (*)(char *)> bdefFunc1;
        typedef bdef_Function<int (*)(void *)> bdefFunc2;

#define TEST(T, U, EXP) testIsReferenceCompatible<T, U, EXP, __LINE__>()

        // All tests perform steps 1-3 and 4

        //   T             U              EXP
        //   ============  =============  =====
        // Step 1
        TEST(int, int, true);
        TEST(bslFunc1, bslFunc1, true);
        TEST(bdefFunc1, bdefFunc1, true);
        TEST(BaseClass, BaseClass, true);
        TEST(DerivedClass, DerivedClass, true);

        // Step 2
        TEST(int, short, false);
        TEST(DerivedClass, BaseClass, false);
        TEST(BaseClass, DerivedClass, false);
        TEST(bdefFunc1, bdefFunc2, false);

        // Step 3
        TEST(bdefFunc1, bslFunc1, true);
        TEST(bdefFunc2, bslFunc1, false);
        TEST(bslFunc1, bdefFunc1, false);

#undef TEST

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the basic 'Function_IsReferenceCompatible' functionality
        //:   works as intended.
        //
        // Plan:
        //: 1 Exercise basic use of 'Function_IsReferenceCompatible'
        //
        // Testing:
        //  BREATHING TEST
        //
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        ASSERT((bslstl::Function_IsReferenceCompatible<int, int>::value));
        ASSERT(
            !(bslstl::Function_IsReferenceCompatible<const int, int>::value));

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
// Copyright 2020 Bloomberg Finance L.P.
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
