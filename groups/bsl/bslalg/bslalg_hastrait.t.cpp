// bslalg_hastrait.t.cpp                                              -*-C++-*-

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bslma_testallocator.h>
#include <bsls_objectbuffer.h>
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
// 'bslalg::HasTrait' is tested in a similar fashion to 'bslalg_TypeTraits'.
// In fact, 'bslalg_TypeTraits' has a replicate of 'bslalg::HasTrait' within
// its test driver.  This is because the effects of applying a trait to a class
// can only be tested by an observer that attempts to use or look at the
// traits, which is precisely what 'bslalg::HasTrait' does.
//
// Therefore, this component will have the exact same test driver as
// 'bslalg_TypeTraits', but with the private 'HasTrait' class within
// 'bslalg_TypeTraits' removed to test the true implementation of
// 'bslalg::HasTrait'.
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

    result |= bslalg::HasTrait<TYPE, bslalg::TypeTraitHasPointerSemantics>::VALUE
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
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() == (TRAIT_BITS)); \
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() == traits);     \
}

//=============================================================================
//                          HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct my_Class0
{
    // Class with no defined traits.
};

struct my_Class1
{
    // Class that uses explicitly-specialized type traits.
};

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class1> : bsl::true_type { };

}  // close bslma namespace
}  // close enterprise namespace

template <class T>
struct my_Class2
{
    // Class template that has nested type traits
    BSLALG_DECLARE_NESTED_TRAITS3(my_Class2,
                                bslalg::TypeTraitBitwiseCopyable,
                                bslalg::TypeTraitBitwiseMoveable,
                                bslalg::TypeTraitHasTrivialDefaultConstructor);
};

struct my_Class4
{
    // Class with no special traits but has conversion from 'void*'.
    // Used to check against false positives for 'bslma::Allocator*' traits.
    my_Class4(void*);
};

struct my_Class5
{
    // Class with no special traits but has conversion from anything.  Used
    // the check against false positives for nested traits and
    // 'bslma::Allocator*' traits.
    template <class T> my_Class5(const T& t);
    template <class T> my_Class5(const volatile T& t);

#if defined(BSLS_PLATFORM_CMP_IBM) || defined(BSLS_PLATFORM_OS_LINUX)
    // Workaround for AIX xlC 6.0 and and Linux gcc compilers.  Without this
    // declaration, the compiler tries to instantiate the templated
    // constructors when probing for 'bslma::Allocator*' conversions.  This
    // declaration short-circuits the traits-sniffing logic so that it will
    // not probe for conversion from 'bslma::Allocator*'.
    BSLALG_DECLARE_NESTED_TRAITS(my_Class5, bslalg::TypeTraitNil);
#endif
};

// Implementations of my_Class5 constructors.  If these constructors are
// actually instantiated, they will fail to compile.
template <class T> my_Class5::my_Class5(const T& t) { t->foo(); }
template <class T> my_Class5::my_Class5(const volatile T& t) { t->foo(); }

enum my_Enum
{
    // Enumeration type (is automatically bitwise copyable)
    MY_ENUM_0
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

        // Nil traits
        TRAIT_TEST(my_Class0, TRAIT_NIL);
        TRAIT_TEST(my_Class4, TRAIT_NIL);
        TRAIT_TEST(my_Class5, TRAIT_NIL);

        // Reference traits.  (Cannot use TRAIT_TEST for references.)
        ASSERT(traitBits<int&>() == TRAIT_NIL);
        ASSERT(traitBits<const int&>() == TRAIT_NIL);
        ASSERT(traitBits<volatile int&>() == TRAIT_NIL);
        ASSERT(traitBits<const volatile int&>() == TRAIT_NIL);
        ASSERT(traitBits<char*&>() == TRAIT_NIL);
        ASSERT(traitBits<char* const&>() == TRAIT_NIL);
        ASSERT(traitBits<const char*&>() == TRAIT_NIL);
        ASSERT(traitBits<const char* const&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Enum&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Class1&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Class2<int>&>() == TRAIT_NIL);

        // Autodetected fundamental traits
        TRAIT_TEST(char, TRAIT_EQPOD);
        TRAIT_TEST(unsigned char, TRAIT_EQPOD);
        TRAIT_TEST(signed char, TRAIT_EQPOD);
        TRAIT_TEST(short, TRAIT_EQPOD);
        TRAIT_TEST(unsigned short, TRAIT_EQPOD);
        TRAIT_TEST(int, TRAIT_EQPOD);
        TRAIT_TEST(unsigned int, TRAIT_EQPOD);
        TRAIT_TEST(bsls::Types::Int64, TRAIT_EQPOD);
        TRAIT_TEST(bsls::Types::Uint64, TRAIT_EQPOD);
        TRAIT_TEST(float, TRAIT_EQPOD);
        TRAIT_TEST(double, TRAIT_EQPOD);
        TRAIT_TEST(char*, TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(const char*, TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(void*, TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(const void*, TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(void* const, TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(my_Enum, TRAIT_EQPOD);
        TRAIT_TEST(int (*)(int), TRAIT_EQPOD | TRAIT_HASPOINTERSEMANTICS);
        TRAIT_TEST(int (my_Class1::*)(int), TRAIT_EQPOD);

        // Explicit traits
        TRAIT_TEST(my_Class1, TRAIT_USESBSLMAALLOCATOR);
        TRAIT_TEST(my_Class2<int>, TRAIT_POD);

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
