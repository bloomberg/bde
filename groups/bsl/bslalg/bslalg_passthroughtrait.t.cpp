// bslalg_passthroughtrait.t.cpp                  -*-C++-*-

#include <bslalg_passthroughtrait.h>

#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraits.h>
#include <bslalg_typetraitusesbslmaallocator.h>

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
// 'bslalg_PassthroughTrait' is used to simplify the declaration of traits.
// Since it's purpose is to passthrough a trait from one type to another, it
// does not matter what trait is used.  'bslalg_TypeTraitBitwiseMoveable' and
// 'bslalg_TypeTraitUsesBslmaAllocator' will be used in this case.
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
const unsigned TRAIT_POD = (TRAIT_BITWISECOPYABLE |
                            TRAIT_BITWISEMOVEABLE |
                            TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR);
const unsigned TRAIT_EQPOD = (TRAIT_POD |
                              TRAIT_BITWISEEQUALITYCOMPARABLE);

template <typename TYPE>
unsigned traitBits()
{
    unsigned result = TRAIT_NIL;

    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitBitwiseMoveable>::VALUE
            ? TRAIT_BITWISEMOVEABLE
            : 0;
    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitBitwiseCopyable>::VALUE
            ? TRAIT_BITWISECOPYABLE
            : 0;
    result |= bslalg_HasTrait<TYPE,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
            ? TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR
            : 0;
    result |= bslalg_HasTrait<TYPE,
                              bslalg_TypeTraitBitwiseEqualityComparable>::VALUE
            ? TRAIT_BITWISEEQUALITYCOMPARABLE
            : 0;
    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitPair>::VALUE
            ? TRAIT_PAIR
            : 0;
    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE
            ? TRAIT_USESBSLMAALLOCATOR
            : 0;
    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASSTLITERATORS
            : 0;
    result |= bslalg_HasTrait<TYPE, bslalg_TypeTraitHasStlIterators>::VALUE
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

// Test that 'traitBits<TYPE>()' does not return the value of 'TRAIT_BITS'
// for every combination of cv-qualified 'TYPE' and reference to 'TYPE'.
#define TRAIT_TEST_NOT(TYPE, TRAIT_BITS) {                             \
    typedef Identity<TYPE >::Type Type;                                \
    typedef Type const          cType;                                 \
    typedef Type volatile       vType;                                 \
    typedef Type const volatile cvType;                                \
    static const char *TypeName = #TYPE;                               \
    static const unsigned traits = traitBits<  Type>();                \
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() != TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() != TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() != TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() != TRAIT_BITS); \
}

//=============================================================================
//                          HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct my_BitwiseMoveableClass {
    BSLALG_DECLARE_NESTED_TRAITS(my_BitwiseMoveableClass,
                                 bslalg_TypeTraitBitwiseMoveable);
};

struct my_UsesBslmaAllocatorClass {
    BSLALG_DECLARE_NESTED_TRAITS(my_UsesBslmaAllocatorClass,
                                 bslalg_TypeTraitUsesBslmaAllocator);
};

struct my_PassthroughBitwiseMoveableClass {
    // This class passthrough the 'bslalg_TypeTraitBitwiseMoveable' from
    // 'my_BitwiseMoveableClass'.
    typedef bslalg_PassthroughTrait<
                my_BitwiseMoveableClass,
                bslalg_TypeTraitBitwiseMoveable>::Type                   Trait;

    BSLALG_DECLARE_NESTED_TRAITS(my_PassthroughBitwiseMoveableClass,
                                 Trait);
};

struct my_PassthroughUsesBslmaAllocatorClass {
    // This class passthrough the 'bslalg_TypeTraitUsesBslmaAllocator' from
    // 'my_UsesBslmaAllocatorClass'.
    typedef bslalg_PassthroughTrait<
                my_UsesBslmaAllocatorClass,
                bslalg_TypeTraitUsesBslmaAllocator>::Type                Trait;

    BSLALG_DECLARE_NESTED_TRAITS(my_PassthroughUsesBslmaAllocatorClass,
                                 Trait);
};

struct my_PassthroughIncorrectBitwiseMoveableClass {
    // This class attempts to passthrough a trait that is not declared.
    typedef bslalg_PassthroughTrait<
                my_UsesBslmaAllocatorClass,
                bslalg_TypeTraitBitwiseMoveable>::Type                   Trait;

    BSLALG_DECLARE_NESTED_TRAITS(my_PassthroughIncorrectBitwiseMoveableClass,
                                 Trait);
};

struct my_PassthroughIncorrectUsesBslmaAllocatorClass {
    // This class attempts to passthrough a trait that is not declared.
    typedef bslalg_PassthroughTrait<
                my_BitwiseMoveableClass,
                bslalg_TypeTraitUsesBslmaAllocator>::Type                Trait;

    BSLALG_DECLARE_NESTED_TRAITS(my_PassthroughIncorrectUsesBslmaAllocatorClass,
                                 Trait);
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
        //   1. That 'bslalg_PassthroughTrait' correctly passes the specified
        //      trait from one type to another if the specified trait is
        //      indeed declared in the original type.
        //
        //   2. That 'bslalg_PassthroughTrait' does not pass the specified
        //      trait and any other traits declared for the original type to
        //      another if the specified trait is not declared in the original
        //      type.
        //
        // Plan:
        //   Using two classes that have the traits
        //   'bslalg_TypeTraitBitwiseMoveable' and
        //   'bslalg_TypeTraitUsesBslmaAllocator'.  Then, using
        //   'bslalg_PassthroughTrait', create four classes: two with each
        //   trait properly propagated, and two with each trait incorrectly
        //   propagated.
        //
        // Testing:
        //   bslalg_PassThroughTrait<typename T, typename TRAIT>::Type
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        TRAIT_TEST(my_PassthroughBitwiseMoveableClass, TRAIT_BITWISEMOVEABLE);

        TRAIT_TEST(my_PassthroughUsesBslmaAllocatorClass,
                   TRAIT_USESBSLMAALLOCATOR);

        TRAIT_TEST_NOT(my_PassthroughIncorrectUsesBslmaAllocatorClass,
                       TRAIT_USESBSLMAALLOCATOR);

        TRAIT_TEST_NOT(my_PassthroughIncorrectUsesBslmaAllocatorClass,
                       TRAIT_BITWISEMOVEABLE);

        TRAIT_TEST_NOT(my_PassthroughIncorrectBitwiseMoveableClass,
                       TRAIT_BITWISEMOVEABLE);

        TRAIT_TEST_NOT(my_PassthroughIncorrectBitwiseMoveableClass,
                       TRAIT_USESBSLMAALLOCATOR);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
