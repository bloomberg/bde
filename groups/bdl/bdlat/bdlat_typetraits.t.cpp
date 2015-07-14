// bdlat_typetraits.t.cpp                  -*-C++-*-

#include <bdlat_typetraits.h>

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

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

// traitless class

struct my_ClassWithNoTraits {
    // Class with no declared traits.
};

// choice types

struct my_ClassWithBasicChoiceTraits {
};
BDLAT_DECL_CHOICE_TRAITS(my_ClassWithBasicChoiceTraits)

struct my_ClassWithBasicChoiceAllocTraits {
};
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(
                         my_ClassWithBasicChoiceAllocTraits)

struct my_ClassWithBasicChoiceBitwiseTraits {
};
BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(
                         my_ClassWithBasicChoiceBitwiseTraits)

struct my_ClassWithBasicChoiceAllocBitwiseTraits {
};
BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                         my_ClassWithBasicChoiceAllocBitwiseTraits)

// sequence types

struct my_ClassWithBasicSequenceTraits {
};
BDLAT_DECL_SEQUENCE_TRAITS(my_ClassWithBasicSequenceTraits)

struct my_ClassWithBasicSequenceAllocTraits {
};
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(
                           my_ClassWithBasicSequenceAllocTraits)

struct my_ClassWithBasicSequenceBitwiseTraits {
};
BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicSequenceBitwiseTraits)

struct my_ClassWithBasicSequenceAllocBitwiseTraits {
};
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicSequenceAllocBitwiseTraits)

// enumeration types

struct my_EnumWithNoTraits {
    enum Value { enumerator };
    // Enumeration with no declared traits.
};

struct my_EnumWithBasicEnumTraits {
    enum Value { enumerator };
};
BDLAT_DECL_ENUMERATION_TRAITS(my_EnumWithBasicEnumTraits)

// customized types

struct my_ClassWithBasicCustomizedTypeTraits {
};
BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(my_ClassWithBasicCustomizedTypeTraits)

struct my_ClassWithBasicCustomizedTypeAllocTraits {
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(
                                 my_ClassWithBasicCustomizedTypeAllocTraits)

struct my_ClassWithBasicCustomizedTypeBitwiseTraits {
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicCustomizedTypeBitwiseTraits)

struct my_ClassWithBasicCustomizedTypeAllocBitwiseTraits {
};
BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
                           my_ClassWithBasicCustomizedTypeAllocBitwiseTraits)

}  // close namespace BloombergLP

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

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
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithNoTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tChoice types." << endl;

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicChoiceAllocBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tSequence types." << endl;

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicSequenceAllocBitwiseTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tEnum types." << endl;

        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithNoTraits::Value,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_EnumWithBasicEnumTraits::Value,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        if (verbose) cout << "\tCustomized types." << endl;

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdeat_TypeTraitBasicSequence>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bdlb::TypeTraitHasPrintMethod>::VALUE));
        ASSERT(! (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                   bslalg::TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg::HasTrait<my_ClassWithBasicCustomizedTypeAllocTraits,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                                  my_ClassWithBasicCustomizedTypeBitwiseTraits,
                                  bdeat_TypeTraitBasicSequence>::VALUE));
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
                             bdeat_TypeTraitBasicChoice>::VALUE));
        ASSERT(  (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdeat_TypeTraitBasicCustomizedType>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdeat_TypeTraitBasicEnumeration>::VALUE));
        ASSERT(! (bslalg::HasTrait<
                             my_ClassWithBasicCustomizedTypeAllocBitwiseTraits,
                             bdeat_TypeTraitBasicSequence>::VALUE));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
