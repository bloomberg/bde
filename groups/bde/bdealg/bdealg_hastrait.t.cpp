// bdealg_hastrait.t.cpp                  -*-C++-*-

#include <bdealg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bslma_testallocator.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// This test driver is not complete.  In particular all the testing is
// performed in the breathing test.  There is a very good infrastructure for
// testing, computing the traits of a type as a bit-field, which should be
// reused systematically.
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
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
const unsigned TRAIT_USESBDEMAALLOCATOR           = 0x0020;
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

    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitBitwiseMoveable>::VALUE
            ? TRAIT_BITWISEMOVEABLE
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitBitwiseCopyable>::VALUE
            ? TRAIT_BITWISECOPYABLE
            : 0;
    result |= bdealg_HasTrait<TYPE,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
            ? TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR
            : 0;
    result |= bdealg_HasTrait<TYPE,
                              bslalg_TypeTraitBitwiseEqualityComparable>::VALUE
            ? TRAIT_BITWISEEQUALITYCOMPARABLE
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitPair>::VALUE
            ? TRAIT_PAIR
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE
            ? TRAIT_USESBDEMAALLOCATOR
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASSTLITERATORS
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitHasStlIterators>::VALUE
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
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() == traits);     \
}

//=============================================================================
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

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
