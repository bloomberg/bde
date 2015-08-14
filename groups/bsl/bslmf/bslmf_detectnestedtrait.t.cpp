// bslmf_detectnestedtrait.t.cpp                                      -*-C++-*-

#include <bslmf_detectnestedtrait.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>

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
// [ *] DetectNestedTrait<>::type
// [ *] DetectNestedTrait<>::value
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
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
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                        CLASSES FOR BREATHING TEST
// ----------------------------------------------------------------------------

// Note curiously-recurring template pattern
template <class TYPE>
struct IsInflatable :
    bslmf::DetectNestedTrait<TYPE, IsInflatable>::type
{
};

struct InflatableType
{
    BSLMF_NESTED_TRAIT_DECLARATION(InflatableType, IsInflatable);
};

struct NonInflatableType
{
};

template <class TYPE>
struct Container
{
    // 'Container' is inflatable iff 'TYPE' is inflatable.
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, IsInflatable,
                                      IsInflatable<TYPE>::value);
};

struct ConvertibleToAny
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.
{
    template <class T>
    operator T() const { return T(); }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a Custom Trait
/// - - - - - - - - - - - - - - - - -
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
// First, in package 'abcd', define a trait, 'RequiresLockTrait', that
// indicates that a type's methods must not be called unless a known lock it
// first acquired:
//..
    namespace abcd {

    template <class TYPE>
    struct RequiresLockTrait :
                      bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type {
    };

    }  // close namespace abcd
//..
// Notice that 'RequiresLockTrait' derives from
// 'bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type' using the
// curiously recurring template pattern.
//
// Then, in package 'xyza', we declare a type, 'DoesNotRequireALockType', that
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
// Next, we declare a type, 'RequiresLockTypeA', that does require the lock.
// We use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with
// the 'abcd::RequiresLockTrait' trait:
//..
    class RequiresLockTypeA {
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(RequiresLockTypeA,
                                       abcd::RequiresLockTrait);

        // CREATORS
        RequiresLockTypeA();
            // ...

    };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a templatized container type, 'Container', that is
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
//..
// Next, we show that traits based on 'bslmf::DetectNestedTrait' can be
// associated with a type using "C++11-style" trait association.  To do this,
// we declare a type, 'RequiresLockTypeB', that also requires the lock, but
// does not used the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
//..
    class RequiresLockTypeB {
        // ...

      public:
        // CREATORS
        RequiresLockTypeB();
            // ...

    };

    }  // close namespace xyza
//..
// Then, we associate 'RequiresLockTypeB' with 'abcd::RequiresLockTrait' by
// directly specializing 'abcd::RequiresLockTrait<xyza::RequiresLockTypeB>'.
// This is the standard way of associating a type with a trait since C++11:
//..
    namespace abcd {

    template <>
    struct RequiresLockTrait<xyza::RequiresLockTypeB> : bsl::true_type {
    };

    }  // close namespace abcd
//..
// Now, we can write a function that inspects
// 'abcd::RequiresLockTrait<TYPE>::value' to test whether or not various types
// are associated with 'abcd::RequiresLockTrait':
//..
    void example1()
    {
        ASSERT(false ==
               abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);

        ASSERT(false ==
               abcd::RequiresLockTrait<
                       xyza::Container<xyza::DoesNotRequireLockType> >::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeA> >::value);

        ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeB> >::value);

        // ...
    }
//..
// Finally, we demonstrate that the trait can be tested at compilation time, by
// writing a function that tests the trait within the context of a compile-time
// 'BSLMF_ASSERT':
//..
    void example2()
    {
        BSLMF_ASSERT(false ==
               abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);

        BSLMF_ASSERT(false ==
               abcd::RequiresLockTrait<
                       xyza::Container<xyza::DoesNotRequireLockType> >::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeA> >::value);

        BSLMF_ASSERT(true  ==
               abcd::RequiresLockTrait<
                            xyza::Container<xyza::RequiresLockTypeB> >::value);

    }
//..

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

    (void)             veryVerbose;
    (void)         veryVeryVerbose;
    (void)     veryVeryVeryVerbose;

    switch (test) { case 0:  // Zero is always the leading case.
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        example1();
        example2();

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
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT((  bslmf::DetectNestedTrait<InflatableType,
                                           IsInflatable>::value));
        ASSERT((! bslmf::DetectNestedTrait<NonInflatableType,
                                           IsInflatable>::value));
        ASSERT((  bslmf::DetectNestedTrait<Container<InflatableType>,
                                           IsInflatable>::value));
        ASSERT((! bslmf::DetectNestedTrait<Container<NonInflatableType>,
                                           IsInflatable>::value));
        ASSERT((! bslmf::DetectNestedTrait<void, IsInflatable>::value));

        ASSERT((  IsInflatable<InflatableType>::value));
        ASSERT((! IsInflatable<NonInflatableType>::value));
        ASSERT((  IsInflatable<Container<InflatableType> >::value));
        ASSERT((! IsInflatable<Container<NonInflatableType> >::value));
        ASSERT((! IsInflatable<void>::value));

        ASSERT((! bslmf::DetectNestedTrait<ConvertibleToAny,
                                           IsInflatable>::value));

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
