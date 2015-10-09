// bslalg_selecttrait.t.cpp                                           -*-C++-*-
#include <bslalg_selecttrait.h>

#include <bslalg_typetraitbitwisecopyable.h>               // for testing only
#include <bslalg_typetraitbitwiseequalitycomparable.h>     // for testing only
#include <bslalg_typetraitbitwisemoveable.h>               // for testing only
#include <bslalg_typetraithasstliterators.h>               // for testing only
#include <bslalg_typetraithastrivialdefaultconstructor.h>  // for testing only
#include <bslalg_typetraitpair.h>                          // for testing only
#include <bslalg_typetraitusesbslmaallocator.h>            // for testing only

#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// There is a very good infrastructure for testing, computing the traits of a
// type as a bit-field from the test driver of 'bslalg_typetraits', which can
// be reused systematically.
//
// 'bslalg::SelectTrait' is used to select a particular trait from a list of
// candidate traits for a particular type.  It does not matter which trait is
// used for testing.  'bslalg::TypeTraitBitwiseMoveable' and
// 'bslalg::TypeTraitUsesBslmaAllocator' will be used in this case.
//-----------------------------------------------------------------------------
// [1] BREATHING TEST

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Numeric values corresponding to trait types.
// Can be ANDed together to represent multiple traits.
const unsigned TRAIT_NIL                          = 0x0000;
const unsigned TRAIT_BITWISEMOVEABLE              = 0x0001;
const unsigned TRAIT_BITWISECOPYABLE              = 0x0002;
const unsigned TRAIT_BITWISEEQUALITYCOMPARABLE    = 0x0004;
const unsigned TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR = 0x0008;
const unsigned TRAIT_PAIR                         = 0x0010;
const unsigned TRAIT_USESBSLMAALLOCATOR           = 0x0020;
const unsigned TRAIT_HASSTLITERATORS              = 0x0040;
const unsigned TRAIT_HASPOINTERSEMANTICS          = 0x0080;

// Traits group
const unsigned TRAIT_POD = (TRAIT_BITWISEMOVEABLE |
                            TRAIT_BITWISECOPYABLE |
                            TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR);
const unsigned TRAIT_EQPOD = (TRAIT_POD |
                              TRAIT_BITWISEEQUALITYCOMPARABLE);

template <class TYPE>
unsigned traitBits()
{
    unsigned result = TRAIT_NIL;

    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitBitwiseMoveable>::VALUE
            ? TRAIT_BITWISEMOVEABLE
            : 0;
    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitBitwiseCopyable>::VALUE
            ? TRAIT_BITWISECOPYABLE
            : 0;
    result |= bslalg::HasTrait<TYPE,
                          bslalg::TypeTraitHasTrivialDefaultConstructor>::VALUE
            ? TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR
            : 0;
    result |= bslalg::HasTrait<TYPE,
                             bslalg::TypeTraitBitwiseEqualityComparable>::VALUE
            ? TRAIT_BITWISEEQUALITYCOMPARABLE
            : 0;
    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitPair>::VALUE
            ? TRAIT_PAIR
            : 0;
    result |= bslalg::HasTrait<TYPE,
                               bslalg::TypeTraitUsesBslmaAllocator>::VALUE
            ? TRAIT_USESBSLMAALLOCATOR
            : 0;
    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASSTLITERATORS
            : 0;
    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASPOINTERSEMANTICS
            : 0;
    return result;
}

template <class TYPE>
struct Identity
{
    // Use this struct to convert a cast-style type (e.g., 'void (*)(int)')
    // into a named type (e.g., 'void (*Type)(int)').
    // Example: 'Identity<void (*)(int)>::Type'.

    typedef TYPE Type;
};

// Test that 'traitBits<TYPE>()' returns the value 'TRAIT_BITS' for every
// combination of cv-qualified 'TYPE' and reference to 'TYPE'.
#define TRAIT_TEST(TYPE, TRAIT_BITS) {                                 \
    typedef Identity<TYPE >::Type Type;                                \
    typedef Type const          cType;                                 \
    typedef Type volatile       vType;                                 \
    typedef Type const volatile cvType;                                \
    static const char *TypeName = #TYPE;                               \
    static const unsigned traits = traitBits<  Type>();                \
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() == TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() == TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() == TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() == TRAIT_BITS); \
}

//=============================================================================
//                          HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct my_NilTraitClass {
};

struct my_BitwiseMoveableClass {
    BSLMF_NESTED_TRAIT_DECLARATION(my_BitwiseMoveableClass,
                                   bslmf::IsBitwiseMoveable);
};

struct my_UsesBslmaAllocatorClass {
    BSLMF_NESTED_TRAIT_DECLARATION(my_UsesBslmaAllocatorClass,
                                   bslma::UsesBslmaAllocator);
};

struct my_BothTraitsClass {
    BSLMF_NESTED_TRAIT_DECLARATION(my_BothTraitsClass,
                                   bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(my_BothTraitsClass,
                                   bslma::UsesBslmaAllocator);
};

namespace BloombergLP {
namespace bslmf {

// Being empty, 'my_NilTraitClass' would normally be implicitly bitwise
// moveable.  Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_NilTraitClass> : bsl::false_type { };

}  // close bslmf namespace
}  // close enterprise namespace

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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the proper trait is selected from multiple traits.
        //   2. That the proper trait is selected regardless of where the
        //      proper trait is specified in the list of traits.
        //   3. That the order of traits listed in 'bslalg::SelectTrait'
        //      reflects the precedence of traits selection.
        //   4. That candidate traits doesn't get passed onto the class when
        //      none of the traits matched.
        //
        // Plan:
        //   Using four helper classes, 'my_NilTraitClass',
        //   'my_BitwiseMoveableClass', 'my_UsesBslmaAllocatorClass',
        //   'my_BothTraitsClass', check that the type traits being assigned
        //   to them are correct using 'bslalg::SelectTrait' with various
        //   ordering according to the list of concerns.
        //
        // Testing:
        //   bslalg::SelectTrait<typename TYPE, typename TRAIT1...>::Type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Regular select
        {
            typedef bslalg::SelectTrait<
                        my_BitwiseMoveableClass,
                        bslalg::TypeTraitBitwiseMoveable>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }

        {
            typedef bslalg::SelectTrait<
                        my_UsesBslmaAllocatorClass,
                        bslalg::TypeTraitUsesBslmaAllocator>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));
        }

        {
            typedef bslalg::SelectTrait<
                        my_BitwiseMoveableClass,
                        bslalg::TypeTraitUsesBslmaAllocator,
                        bslalg::TypeTraitBitwiseMoveable>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }

        // Test the order of traits
        {
            typedef bslalg::SelectTrait<
                              my_BothTraitsClass,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitBitwiseMoveable>::Type TestType;

            ASSERT((1 == bsl::is_same<TestType,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));
        }
        {
            typedef bslalg::SelectTrait<
                           my_BothTraitsClass,
                           bslalg::TypeTraitBitwiseMoveable,
                           bslalg::TypeTraitUsesBslmaAllocator>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }
        {
            typedef bslalg::SelectTrait<
                              my_BitwiseMoveableClass,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitBitwiseMoveable>::Type TestType;

            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }
        {
            typedef bslalg::SelectTrait<
                              my_BitwiseMoveableClass,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitBitwiseMoveable>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }
        {
            typedef bslalg::SelectTrait<
                              my_BitwiseMoveableClass,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitBitwiseMoveable>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }
        {
            typedef bslalg::SelectTrait<
                              my_BitwiseMoveableClass,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitUsesBslmaAllocator,
                              bslalg::TypeTraitBitwiseMoveable>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                    bslalg::TypeTraitBitwiseMoveable>::VALUE));
        }

        // Test that traits wouldn't be added on when there's no match.
        {
            typedef bslalg::SelectTrait<
                           my_NilTraitClass,
                           bslalg::TypeTraitBitwiseMoveable,
                           bslalg::TypeTraitUsesBslmaAllocator>::Type TestType;
            ASSERT((1 == bsl::is_same<TestType,
                                      bslalg::TypeTraitNil>::VALUE));
        }

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
