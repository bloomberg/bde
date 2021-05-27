// bslmf_isreferencewrapper.t.cpp                                     -*-C++-*-
#include <bslmf_isreferencewrapper.h>

#include <bsla_maybeunused.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_review.h>

#include <stdio.h>
#include <stdlib.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines a boolean metafunction,
// 'bslmf::IsReferenceWrapper', that is intended to determine whether a
// template parameter type is a specialization of 'bsl::reference_wrapper'.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslmf::IsReferenceWrapper
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT         BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV        BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT    BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT   BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT   BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT   BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT   BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT   BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT   BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT   BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

#define Q              BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P              BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_             BSLS_BSLTESTUTIL_P_  // P(X) without '\n.'
#define T_             BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_             BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                           ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {


                          // ======================
                          // class ReferenceWrapper
                          // ======================

template <class TYPE>
class ReferenceWrapper {
    // This class template provides an emulation of the minimal part of
    // 'bsl::reference_wrapper' needed for testing.

    // This class template specializes 'BloombergLP::bslmf::IsReferenceWrapper'
    // to inherit from 'bsl::true_type' for any specified 'TYPE'.

  public:
    // TYPES
    typedef TYPE type;
};

                              // ===============
                              // class ClassType
                              // ===============

class ClassType;
    // This class provides an incomplete class type.

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslmf {

template <class TYPE>
struct IsReferenceWrapper<u::ReferenceWrapper<TYPE> > : bsl::true_type {
    // This partial 'IsReferenceWrapper' specialization publicly inherits from
    // inherits from 'bsl::true_type' for any specialization of
    // 'u::ReferenceWrapper'.  Note that this specialization is provided to
    // test that the 'IsReferenceWrapper' trait is specializable for user
    // class templates.
};

}  // close namespace bslmf
}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace {
using namespace BloombergLP;

///Usage
///-----
// In this section we show intended use of this component.
//
///Example: Reference Access
///- - - - - - - - - - - - -
// In this example, we suppose that we would like to provide a software
// component that treats specializations of 'bsl::reference_wrapper' and
// true reference qualified types in the same way.
//
// Suppose that we would like to do this by having a utility function that
// returns an lvalue reference to an object given either an lvalue-reference
// qualified type or a 'bsl::reference_wrapper'.
//
// First, we define a utility 'struct' that contains the overload of this
// utility function for 'const' and non-'const' lvalue-reference qualified
// types:
//..
    struct MyLvalueReferenceUtil {
        // CLASS METHODS
        template <class TYPE>
        static TYPE& access(TYPE& reference)
        {
            return reference;
        }

        template <class TYPE>
        static const TYPE& access(const TYPE& reference)
        {
            return reference;
        }
//..
// Then, we define the overload of this utility function for reference
// wrappers, taking care to define it such that it does not participate in
// overload resolution unless it is passed a reference wrapper:
//..
        template <class TYPE>
        static typename bsl::enable_if<
            bslmf::IsReferenceWrapper<TYPE>::value,
            typename bsl::add_lvalue_reference<typename TYPE::type>::type
        >::type
        access(TYPE referenceWrapper)
        {
            return referenceWrapper.get();
        }
    };
//..
// Finally, we can verify that this utility allows us to transparently access
// lvalue references:
//..
    void example()
    {
        int x = 1;
        ASSERT(1 == MyLvalueReferenceUtil::access(x));

        const int y = 2;
        ASSERT(2 == MyLvalueReferenceUtil::access(y));

        // Note that even though the following invokes 'access' with the
        // literal 3, which is a prvalue expression, the call expression is
        // an lvalue with type 'const int&'.
        ASSERT(3 == MyLvalueReferenceUtil::access(3));

        // Further, note that the levelization of the
        // 'bslmf_isreferencewrapper' component prohibits showing the
        // application of 'MyLvalueReferenceUtil' to a reference wrapper.
        // The following commented-out code would be valid given a suitable
        // 'bsl::reference_wrapper' type that acts like a reference wrapper
        // and specializes the 'bslmf::IsReferenceWrapper' trait accordingly.
        //..
        //  assert(x == MyLvalueReferenceUtil::access(
        //                                    bsl::reference_wrapper<int>(x)));
        //..
    }
//..

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 2: {
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
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");
        }

        example();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST 'bslmf::IsReferenceWrapper'
        //   Ensure that 'bslmf::IsReferenceWrapper' implements a
        //   user-specializable boolean metafunction that is 'false' by
        //   default.
        //
        // Concerns:
        //: 1 The primary 'bslmf::IsReferenceWrapper' template inherits from
        //:   'bsl::false_type'.
        //:
        //: 2 Specializations of 'bslmf::IsReferenceWrapper' can inherit from
        //:   'bsl::true_type'.
        //:
        //: 3 Reference-qualified types are not considered reference wrappers
        //:   by default.
        //
        // Plan:
        //: 1 Instantiate 'bslmf::IsReferenceWrapper' with an arbitrary class
        //:   type, and verify that it is convertible to 'bsl::false_type'.
        //:
        //: 2 Specialize 'bslmf::IsReferenceWrapper' for any specialization of
        //:   a class template, 'u::ReferenceWrapper', to inherit from
        //:   'bsl::true_type'.
        //:
        //: 3 Instantiate 'bslmf::IsReferenceWrapper' with an arbitrary
        //:   'bslmf::IsReferenceWrapper' specialization and verify that it
        //:   is convertible to 'bsl::true_type'.
        //:
        //: 4 Instantiate 'bslmf::IsReferenceWrapper' with some reference-
        //:   qualified types and verify that they are not convertible to
        //:   'bsl::false_type'.
        //
        // Testing:
        //   bslmf::IsReferenceWrapper
        // --------------------------------------------------------------------

        ASSERT((bsl::is_convertible<bslmf::IsReferenceWrapper<u::ClassType>,
                                    bsl::false_type>::value));

        ASSERT((bsl::is_convertible<
                bslmf::IsReferenceWrapper<u::ReferenceWrapper<int> >,
                bsl::true_type>::value));

        ASSERT(!(bsl::is_convertible<
                bslmf::IsReferenceWrapper<u::ReferenceWrapper<int>&>,
                bsl::true_type>::value));

        ASSERT(!(bsl::is_convertible<
                bslmf::IsReferenceWrapper<int&>,
                bsl::true_type>::value));


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
