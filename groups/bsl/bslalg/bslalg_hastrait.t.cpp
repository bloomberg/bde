// bslalg_hastrait.t.cpp                                              -*-C++-*-
#include <bslalg_hastrait.h>

#include <bslalg_typetraitbitwisecopyable.h>               // for testing only
#include <bslalg_typetraitbitwiseequalitycomparable.h>     // for testing only
#include <bslalg_typetraitbitwisemoveable.h>               // for testing only
#include <bslalg_typetraithaspointersemantics.h>           // for testing only
#include <bslalg_typetraithasstliterators.h>               // for testing only
#include <bslalg_typetraithastrivialdefaultconstructor.h>  // for testing only
#include <bslalg_typetraitpair.h>                          // for testing only
#include <bslalg_typetraitusesbslmaallocator.h>            // for testing only

#include <bslma_testallocator.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// 'bslalg_hastrait' is tested in a similar fashion to 'bslalg_typetraits'.
// In fact, 'bslalg_typetraits' has a replicate of 'bslalg::HasTrait' within
// its test driver.  This is because the effects of applying a trait to a class
// can only be tested by an observer that attempts to use or look at the
// traits, which is precisely what 'bslalg::HasTrait' does.
//
// Therefore, this component will have the exact same test driver as
// 'bslalg_typetraits', but with the private 'HasTrait' class within
// 'bslalg_typetraits's test driver removed to test the true implementation of
// 'bslalg::HasTrait'.
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

    result |= bslalg::HasTrait<TYPE,
                               bslalg::TypeTraitHasPointerSemantics>::VALUE
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
#define TRAIT_TEST(TYPE, TRAIT_BITS) {                                   \
    typedef Identity<TYPE >::Type Type;                                  \
    typedef Type const          cType;                                   \
    typedef Type volatile       vType;                                   \
    typedef Type const volatile cvType;                                  \
    static const char *TypeName = #TYPE;                                 \
    static const unsigned traits = traitBits<  Type>();                  \
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() == (TRAIT_BITS)); \
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() == traits);       \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() == traits);       \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() == traits);       \
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
namespace bslmf {

// Being empty, 'my_Class0' would normally be implicitly bitwise moveable.
// Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class0> : bsl::false_type { };

// Being empty, 'my_Class1' would normally be implicitly bitwise moveable.
// Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class1> : bsl::false_type { };

}  // close bslmf namespace

namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class1> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

template <class T>
struct my_Class2
{
    // Class template that has nested type traits.
    BSLMF_NESTED_TRAIT_DECLARATION(my_Class2, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(my_Class2, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(my_Class2,
                                   bsl::is_trivially_default_constructible);
};

struct my_Class4
{
    // Class with no special traits but has conversion from 'void*'.
    // Used to check against false positives for 'bslma::Allocator*' traits.
    my_Class4(void*);
};

enum my_Enum
{
    // Enumeration type (is automatically bitwise copyable)
    MY_ENUM_0
};

struct ConvertibleToAnyNoTraits
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.
{
    template <class T>
    operator T() const { return T(); }
};

struct ConvertibleToAnyWithTraits {
    template <class T>
    operator T() const { return T(); }
};

namespace BloombergLP {
namespace bslmf {

// Being empty, 'my_Class4' would normally be implicitly bitwise
// moveable.  Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class4> : bsl::false_type { };

// Being empty, 'ConvertibleToAnyNoTraits' would normally be implicitly bitwise
// moveable.  Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<ConvertibleToAnyNoTraits> : bsl::false_type { };

}  // close namespace bslmf

namespace bslma {

template <>
struct UsesBslmaAllocator<ConvertibleToAnyWithTraits> : bsl::true_type {
    // Even though the nested trait logic is disabled by the template
    // conversion operator, the out-of-class trait specialization should still
    // work.
};

}  // close namespace bslma
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

        // Trait tests for type convertible to anything
        TRAIT_TEST(ConvertibleToAnyNoTraits, TRAIT_NIL);
        TRAIT_TEST(ConvertibleToAnyWithTraits,
                   TRAIT_USESBSLMAALLOCATOR | TRAIT_BITWISEMOVEABLE);

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
