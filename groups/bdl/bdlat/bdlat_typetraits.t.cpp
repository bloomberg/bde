// bdlat_typetraits.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_typetraits.h>

#include <bslim_testutil.h>

#include <bslalg_hastrait.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {

// traitless class

struct my_ClassWithNoTraits {
    // Class with no declared traits.
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};

// choice types

struct my_ClassWithBasicChoiceTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CHOICE_TRAITS(my_ClassWithBasicChoiceTraits)

struct my_ClassWithBasicChoiceAllocTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(
                         my_ClassWithBasicChoiceAllocTraits)

struct my_ClassWithBasicChoiceBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(
                         my_ClassWithBasicChoiceBitwiseTraits)

struct my_ClassWithBasicChoiceAllocBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                         my_ClassWithBasicChoiceAllocBitwiseTraits)

// sequence types

struct my_ClassWithBasicSequenceTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_SEQUENCE_TRAITS(my_ClassWithBasicSequenceTraits)

struct my_ClassWithBasicSequenceAllocTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(
                           my_ClassWithBasicSequenceAllocTraits)

struct my_ClassWithBasicSequenceBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicSequenceBitwiseTraits)

struct my_ClassWithBasicSequenceAllocBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicSequenceAllocBitwiseTraits)

// enumeration types

struct my_EnumWithNoTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
    enum Value { enumerator };
    // Enumeration with no declared traits.
};

struct my_EnumWithBasicEnumTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
    enum Value { enumerator };
};
BDLAT_DECL_ENUMERATION_TRAITS(my_EnumWithBasicEnumTraits)

// customized types

struct my_ClassWithBasicCustomizedTypeTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(my_ClassWithBasicCustomizedTypeTraits)

struct my_ClassWithBasicCustomizedTypeAllocTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(
                                 my_ClassWithBasicCustomizedTypeAllocTraits)

struct my_ClassWithBasicCustomizedTypeBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicCustomizedTypeBitwiseTraits)

struct my_ClassWithBasicCustomizedTypeAllocBitwiseTraits {
    char d_s[2];  // Defeat the sizeof(1) TypeTraitBitwiseMoveable assumption.
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicCustomizedTypeAllocBitwiseTraits)

}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tTraitless type." << endl;

        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tChoice types." << endl;

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tSequence types." << endl;

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tEnum types." << endl;

        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tCustomized types." << endl;

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                                 my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdlat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdlat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdlat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdlat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
