// bslalg_hastrait.t.cpp                                              -*-C++-*-
#include <bslalg_hastrait.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// Define some test traits and verify that they can be detected using
// 'bslalg::HasTrait' whether the trait is ascribed to a type using the C++11
// idiom for defining traits or using the 'BSLMF_NESTED_TRAIT_DECLARATION'
// macro.
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
//              GLOBAL TRAITS AND HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Define "base" test traits patterned after 'bslalg::HasStlIterators'.

namespace bslabc {

template <class TYPE>
struct ColorBlue : bslmf::DetectNestedTrait<TYPE, ColorBlue>
{
};

template <class TYPE>
struct ColorGreen : bslmf::DetectNestedTrait<TYPE, ColorGreen>
{
};

}  // close namespace bslabc

// Define "shim" test traits that are compatible with 'bslalg::HasTrait'.

namespace bslxyz {

struct TypeTraitIsBlue {

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslabc::ColorBlue>
    {
    };

    template <class TYPE>
    struct Metafunction : bslabc::ColorBlue<TYPE>::type
    {
    };
};

struct TypeTraitIsGreen {

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslabc::ColorGreen>
    {
    };

    template <class TYPE>
    struct Metafunction : bslabc::ColorGreen<TYPE>::type
    {
    };
};

}  // close namespace bslxyz

// Numeric values corresponding to trait types; can be ANDed together to
// represent multiple traits.

const unsigned TRAIT_NIL   = 0x0000;
const unsigned TRAIT_BLUE  = 0x0001;
const unsigned TRAIT_GREEN = 0x0002;

const unsigned TRAIT_BOTH = TRAIT_BLUE | TRAIT_GREEN;

template <class TYPE>
unsigned traitBits()
{
    unsigned result = TRAIT_NIL;

    result |= bslalg::HasTrait<TYPE, bslxyz::TypeTraitIsBlue>::VALUE
              ? TRAIT_BLUE
              : 0;

    result |= bslalg::HasTrait<TYPE, bslxyz::TypeTraitIsGreen>::VALUE
              ? TRAIT_GREEN
              : 0;

    return result;
}

template <class TYPE>
struct Identity {
    // Use this struct to convert a cast-style type (e.g., 'void (*)(int)')
    // into a named type (e.g., 'void (*Type)(int)').  For example:
    //..
    //  typedef Identity<void (*)(int)>::Type Type;
    //..

    typedef TYPE Type;
};

// Test that 'traitBits<TYPE>()' returns the value 'TRAIT_BITS' for every
// combination of cv-qualified 'TYPE' and reference to 'TYPE'.
#define TRAIT_TEST(TYPE, TRAIT_BITS) {                                   \
    typedef Identity<TYPE>::Type Type;                                   \
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

enum my_Enum {
    MY_ENUM_0
};

struct my_ClassNoTraits {
    // Class with no defined traits.

    int d_dummy;

    my_ClassNoTraits(){}
    my_ClassNoTraits(const my_ClassNoTraits&){}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct my_DerivedNoTraits : my_ClassNoTraits {
    // Derived class with no defined traits.
};

// In the names of the next several test classes:
//: o "NB" means "has Nested Blue trait"
//: o "XG" means "has eXternally defined Green trait" (via C++11 idiom)

struct my_ClassNB {
    BSLMF_NESTED_TRAIT_DECLARATION(my_ClassNB, bslabc::ColorBlue);
};

template <class T>
struct my_ClassNBNG {
    BSLMF_NESTED_TRAIT_DECLARATION(my_ClassNBNG, bslabc::ColorBlue);
    BSLMF_NESTED_TRAIT_DECLARATION(my_ClassNBNG, bslabc::ColorGreen);
};

struct my_ClassXG {
};

struct my_ClassXBXG {
};

struct my_ClassNBXG {
    BSLMF_NESTED_TRAIT_DECLARATION(my_ClassNBXG, bslabc::ColorBlue);
};

struct my_DerivedNBXG : my_ClassNBXG {
};

struct ConvertibleToAnyNoTraits : my_ClassNoTraits {
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.

    template <class T>
    operator T() const { return T(); }
};

struct ConvertibleToAnyWithTraits : my_ClassNoTraits {
    template <class T>
    operator T() const { return T(); }
};

namespace bslabc {

template <>
struct ColorGreen<my_ClassXG> : bsl::true_type {
};

template <>
struct ColorBlue<my_ClassXBXG> : bsl::true_type {
};

template <>
struct ColorGreen<my_ClassXBXG> : bsl::true_type {
};

template <>
struct ColorGreen<my_ClassNBXG> : bsl::true_type {
};

template <>
struct ColorBlue<ConvertibleToAnyWithTraits> : bsl::true_type {
    // Even though the nested trait logic is disabled by the template
    // conversion operator, the out-of-class trait specialization should still
    // work.
};

}  // close namespace bslabc

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
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Nil traits.
        TRAIT_TEST(my_Enum,            TRAIT_NIL);
        TRAIT_TEST(my_ClassNoTraits,   TRAIT_NIL);
        TRAIT_TEST(my_DerivedNoTraits, TRAIT_NIL);

        // Reference traits.  (Cannot use TRAIT_TEST for references.)
        ASSERT(traitBits<int&>()                == TRAIT_NIL);
        ASSERT(traitBits<const int&>()          == TRAIT_NIL);
        ASSERT(traitBits<volatile int&>()       == TRAIT_NIL);
        ASSERT(traitBits<const volatile int&>() == TRAIT_NIL);
        ASSERT(traitBits<char *&>()             == TRAIT_NIL);
        ASSERT(traitBits<char *const&>()        == TRAIT_NIL);
        ASSERT(traitBits<const char *&>()       == TRAIT_NIL);
        ASSERT(traitBits<const char *const&>()  == TRAIT_NIL);
        ASSERT(traitBits<my_Enum&>()            == TRAIT_NIL);
        ASSERT(traitBits<my_ClassNBNG<int>&>()  == TRAIT_NIL);

        // Fundamental traits.
        TRAIT_TEST(char,                TRAIT_NIL);
        TRAIT_TEST(unsigned char,       TRAIT_NIL);
        TRAIT_TEST(signed char,         TRAIT_NIL);
        TRAIT_TEST(short,               TRAIT_NIL);
        TRAIT_TEST(unsigned short,      TRAIT_NIL);
        TRAIT_TEST(int,                 TRAIT_NIL);
        TRAIT_TEST(unsigned int,        TRAIT_NIL);
        TRAIT_TEST(bsls::Types::Int64,  TRAIT_NIL);
        TRAIT_TEST(bsls::Types::Uint64, TRAIT_NIL);
        TRAIT_TEST(float,               TRAIT_NIL);
        TRAIT_TEST(double,              TRAIT_NIL);
        TRAIT_TEST(char *,              TRAIT_NIL);
        TRAIT_TEST(const char *,        TRAIT_NIL);
        TRAIT_TEST(void *,              TRAIT_NIL);
        TRAIT_TEST(const void *,        TRAIT_NIL);
        TRAIT_TEST(void *const,         TRAIT_NIL);
        TRAIT_TEST(my_Enum,             TRAIT_NIL);
        TRAIT_TEST(int (*)(int),        TRAIT_NIL);

        // Explicit traits.
        TRAIT_TEST(my_ClassNB,        TRAIT_BLUE);
        TRAIT_TEST(my_ClassNBNG<int>, TRAIT_BOTH);
        TRAIT_TEST(my_ClassXG,        TRAIT_GREEN);
        TRAIT_TEST(my_ClassXBXG,      TRAIT_BOTH);
        TRAIT_TEST(my_ClassNBXG,      TRAIT_BOTH);
        TRAIT_TEST(my_DerivedNBXG,    TRAIT_NIL);

        // Trait tests for type convertible to anything.
        TRAIT_TEST(ConvertibleToAnyNoTraits,   TRAIT_NIL);
        TRAIT_TEST(ConvertibleToAnyWithTraits, TRAIT_BLUE);

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
