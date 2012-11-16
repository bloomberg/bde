// bslalg_selecttrait.t.cpp                                           -*-C++-*-

#include <bslalg_selecttrait.h>

#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraits.h>
#include <bslalg_typetraitusesbslmaallocator.h>

#include <bslmf_issame.h>

#include <bsls_platform.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// There is a very good infrastructure for testing, computing the traits of a
// type as a bit-field from the test driver of 'bslalg_TypeTraits', which can
// be reused systematically.
//
// 'bslalg::SelectTrait' is used to select a particular trait from a list of
// candidate traits for a particular type.  It does not matter which trait is
// used for testing.  'bslalg::TypeTraitBitwiseMoveable' and
// 'bslalg::TypeTraitUsesBslmaAllocator' will be used in this case.
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) \
    if (!(X)) { printf("%s = %s\n", #I, (I)); aSsErT(!(X), #X, __LINE__); }

# define LOOP2_ASSERT(I,J,X) \
    if (!(X)) { printf("%s = %s, %s = %d\n", #I, (I), #J, (J));  \
                aSsErT(!(X), #X, __LINE__); }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) printf("%s = %d\n", #X, (X)); // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

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

template <typename TYPE>
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

template <typename TYPE>
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
    BSLALG_DECLARE_NESTED_TRAITS(my_BitwiseMoveableClass,
                                 bslalg::TypeTraitBitwiseMoveable);
};

struct my_UsesBslmaAllocatorClass {
    BSLALG_DECLARE_NESTED_TRAITS(my_UsesBslmaAllocatorClass,
                                 bslalg::TypeTraitUsesBslmaAllocator);
};

struct my_BothTraitsClass {
    BSLALG_DECLARE_NESTED_TRAITS2(my_BothTraitsClass,
                                  bslalg::TypeTraitBitwiseMoveable,
                                  bslalg::TypeTraitUsesBslmaAllocator);
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
