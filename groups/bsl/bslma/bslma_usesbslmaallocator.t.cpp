// bslma_usesbslmaallocator.t.cpp                                     -*-C++-*-

#include <bslma_usesbslmaallocator.h>

#include <bslma_allocator.h>
#include <bslmf_assert.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component provides a meta-function for associating a trait with a type
// and detecting whether a trait is associated with a type.
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: -TP19

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

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Suppress some bde_verify warnings for the usage example

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD03  // parameter not documented warning

class DoesNotUseAnAllocatorType {
};

///Usage
///-----
// This section illustrates intended usage of this component.
//
// Example 1: Associating the 'bslma' Allocator Trait with a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to declare two types that make use of a 'bslma' allocator,
// and need to associate the 'UsesBslmaAllocator' trait with those types (so
// that they behave correctly when inserted into a 'bsl' container, for
// example).  In this example we will demonstrate two different mechanisms by
// which a trait may be associated with a type.
//
// First, we declare a type 'UsesAllocatorType1', using the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with the
// 'UsesBslmaAllocator' trait:
//..
    namespace xyz {

    class UsesAllocatorType1 {
        // ...
//
      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
                                       bslma::UsesBslmaAllocator);
        // CREATORS
        explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
            // ...
//
        UsesAllocatorType1(const UsesAllocatorType1&  original,
                       bslma::Allocator              *basicAllocator = 0);
            // ...
    };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a type 'UsesAllocatorType2', and define a specialization of
// the 'UsesBslmaAllocator' trait for 'UsesAllocatorType2' that associates the
// 'UsesBslmaAllocator' trait with the type (note that this is sometimes
// referred to as a "C++11 style" trait declaration, since it is more in
// keeping with the style of trait declarations found in the C++11 standard):
//..
    class UsesAllocatorType2 {
        // ...
//
      public:
        // CREATORS
        explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
            // ...
//
        UsesAllocatorType2(const UsesAllocatorType2&  original,
                           bslma::Allocator          *basicAllocator = 0);
            // ...
    };
//
    }  // close namespace xyz

    namespace BloombergLP {
    namespace bslma {
//
    template <> struct UsesBslmaAllocator<xyz::UsesAllocatorType2> :
                                                                 bsl::true_type
    {};
//
    }  // close package namespace
    }  // close enterprise namespace
//..
// Notice that the specialization must be performed in the 'BloombergLP::bslma'
// namespace.

// BDE_VERIFY pragma: pop
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct SniffUsesBslmaAllocatorFromConstructor
{
    SniffUsesBslmaAllocatorFromConstructor(bslma::Allocator *);     // IMPLICIT
        // Create a 'SniffUsesBslmaAllocatorFromConstructor' object.
};

struct ConstructFromAnyPointer
{
    template <class TYPE>
    ConstructFromAnyPointer(TYPE *);
        // Create a 'ConstructFromAnyPointer' object.
};

struct ClassUsingBslmaAllocator
{
};

struct DerivedAllocator : bslma::Allocator
{
};

struct ConvertibleToAny {
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.

    template <class TYPE>
    operator TYPE() const { return TYPE(); }
        // Return a default constructed object of 'TYPE'.
};

}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ClassUsingBslmaAllocator> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<ConvertibleToAny> : bsl::true_type {
    // Even though the nested trait logic is disabled by the template
    // conversion operator, the out-of-class trait specialization should still
    // work.
};

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        if (verbose) printf("\nMain example usage test.\n");

        using namespace xyz;

///Example 2: Testing Whether a Types Uses a 'bslma' Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to test whether each of a set of different types use a
// 'bslma' allocator.
//
// Here, we use the 'UsesBslmaAllocator' template to test whether the types
// 'DoesNotUseAnAllocatorType', 'UsesAllocatorType1', and 'UsesAllocatorType2'
// (defined above) use allocators:
//..
    ASSERT(false ==
           bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
//
    ASSERT(true  ==
           bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
//
    ASSERT(true  ==
           bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
//..
// Finally, we demonstrate that the trait can be tested at compilation time, by
// testing the trait within the context of a compile-time 'BSLMF_ASSERT':
//..
    BSLMF_ASSERT(false ==
                 bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
//
    BSLMF_ASSERT(true  ==
                 bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
//
    BSLMF_ASSERT(true ==
                 bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //  BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor const>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor volatile>::value);

        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator const>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator volatile>::value);

        ASSERT(!bslma::UsesBslmaAllocator<ConstructFromAnyPointer>::value);
        ASSERT(!bslma::UsesBslmaAllocator<bslma::Allocator *>::value);
        ASSERT(!bslma::UsesBslmaAllocator<bslma::Allocator const *>::value);
        ASSERT(!bslma::UsesBslmaAllocator<bslma::Allocator volatile *>::value);

        ASSERT(!bslma::UsesBslmaAllocator<DerivedAllocator *>::value);

        ASSERT(bslma::UsesBslmaAllocator<ConvertibleToAny>::value);
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
// Copyright 2013 Bloomberg Finance L.P.
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
