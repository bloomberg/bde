// bslmf_nestedtraitdeclaration.t.cpp                                 -*-C++-*-

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// [ *] BSLMF_NESTED_TRAIT_DECLARATION
// [ *] BSLMF_NESTED_TRAIT_DECLARATION_IF
// ----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE

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
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// Define a C++11-style trait, to provide the interface expected by
// 'BSLMF_NESTED_TRAIT_DECLARATION_IF'.

namespace abcd {

template <class TYPE>
struct RequiresLockTrait : bsl::false_type {
};

}  // close namespace abcd

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing a Type for a Custom Trait
/// - - - - - - - - - - - - - - - - - - - - - -
// When writing generic infrastructure code, we often need to choose among
// multiple code paths based on the capabilities of the types on which we are
// operating.  If those capabilities are reflected in a type's public
// interface, we may be able to use techniques such as SFINAE to choose the
// appropriate code path.  However, SFINAE cannot detect all of a type's
// capabilities.  In particular, SFINAE cannot detect constructors, memory
// allocation, thread-safety characteristics, and so on.  Functions that depend
// on these capabilities must use another technique to determine the correct
// code path to use for a given type.  We can solve this sort of problem by
// associating types with custom traits that indicate what capabilities are
// provided by a given type.
//
// First, assume that a compatible trait, 'abcd::RequiresLockTrait', has been
// defined that indicates that a type's methods must not be called unless a
// known lock it first acquired:
//..
    namespace abcd { template <class TYPE> struct RequiresLockTrait; }
//..
// The implementation of 'abcd::RequiresLockTrait' is not shown.
//
// Then, in package 'xyza', we declare a type, 'DoesNotRequireLockType', that
// can be used without acquiring the lock:
//..
    namespace xyza {

    class DoesNotRequireLockType {
        // ...

      public:
        // CREATORS
        DoesNotRequireLockType();
            // ...
    };
//..
// Next, we declare a type, 'RequiresLockType', that does require the lock.  We
// use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with
// the 'abcd::RequiresLockTrait' trait:
//..
    class RequiresLockType {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(RequiresLockType,
                                       abcd::RequiresLockTrait);

        // CREATORS
        RequiresLockType();
            // ...

    };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Now, we declare a templatized container type, 'Container', that is
// parameterized on some 'ELEMENT' type.  If 'ELEMENT' requires a lock, then a
// 'Container' of 'ELEMENT's will require a lock as well.  This can be
// expressed using the 'BSLMF_NESTED_TRAIT_DECLARATION_IF' macro, by providing
// 'abcd::RequiresLockTrait<ELEMENT>::value' as the condition for associating
// the trait with 'Container'.
//..
    template <class ELEMENT>
    struct Container {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, abcd::RequiresLockTrait,
                                      abcd::RequiresLockTrait<ELEMENT>::value);

        // ...
    };

    }  // close namespace xyza
//..
// Finally, code interacting with 'xyza::DoesNotRequireLockType',
// 'xyza::RequiresLockType' or 'xyza::Container' objects will be able to choose
// the appropriate code path by checking for the 'abcd::RequiresLockTrait'
// trait.  See 'bslmf_detectnestedtrait' for an example of how generic code
// would use such a trait.

// Associate 'abcd::RequiresLockTrait' with 'xyza::RequiresLockType', so that
// we can control the condition in 'BSLMF_NESTED_TRAIT_DECLARATION_IF'.

namespace abcd {

template <>
struct RequiresLockTrait<xyza::RequiresLockType> : bsl::true_type {
};

}  // close namespace abcd

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

using namespace BloombergLP;

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 The usage example has no run-time part, because a nested trait
        //:   declaration cannot actually associate a trait with a type unless
        //:   that trait is based on machinery that will be introduced in
        //:   'bslmf_detectnestedtrait'.  To test the usage example, we confirm
        //:   that the syntax presented is correct by checking the intended
        //:   effect of the macros used: that they make a type convertible to a
        //:   'bslmf::NestedTraitDeclaration' (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        using xyza::DoesNotRequireLockType;
        using xyza::RequiresLockType;
        using xyza::Container;

        ASSERT(false ==
               (bsl::is_convertible<DoesNotRequireLockType,
                                    bslmf::NestedTraitDeclaration<
                                           DoesNotRequireLockType,
                                           abcd::RequiresLockTrait> >::value));

        ASSERT(true  ==
               (bsl::is_convertible<RequiresLockType,
                                    bslmf::NestedTraitDeclaration<
                                           RequiresLockType,
                                           abcd::RequiresLockTrait> >::value));

        ASSERT(false ==
               (bsl::is_convertible<Container<DoesNotRequireLockType>,
                                    bslmf::NestedTraitDeclaration<
                                           Container<DoesNotRequireLockType>,
                                           abcd::RequiresLockTrait> >::value));

        ASSERT(true  ==
               (bsl::is_convertible<Container<RequiresLockType>,
                                    bslmf::NestedTraitDeclaration<
                                           Container<RequiresLockType>,
                                           abcd::RequiresLockTrait> >::value));


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
