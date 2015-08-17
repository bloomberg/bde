// bslmf_selecttrait.t.cpp                                            -*-C++-*-

#include <bslmf_selecttrait.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'
#include <string.h>
#include <new>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE> struct IsBool : bsl::false_type { };
template <> struct IsBool<bool>     : bsl::true_type { };

template <class TYPE> struct IsChar : bsl::false_type { };
template <> struct IsChar<char>     : bsl::true_type { };

template <class TYPE> struct IsShort : bsl::false_type { };
template <> struct IsShort<short>    : bsl::true_type { };

template <class TYPE> struct IsLong : bsl::false_type { };
template <> struct IsLong<long>     : bsl::true_type { };

template <class TYPE> struct IsFloat : bsl::false_type { };
template <> struct IsFloat<float>    : bsl::true_type { };

int whichTrait(bslmf::SelectTraitCase<>)        { return 0; }
int whichTrait(bslmf::SelectTraitCase<IsBool>)  { return 1; }
int whichTrait(bslmf::SelectTraitCase<IsChar>)  { return 2; }
int whichTrait(bslmf::SelectTraitCase<IsShort>) { return 3; }
int whichTrait(bslmf::SelectTraitCase<IsLong>)  { return 4; }
int whichTrait(bslmf::SelectTraitCase<IsFloat>) { return 5; }

template <class TYPE>
int breathingTest()
{
    // Use enum to ensure that evaluation is at compile time
    enum {
        RESULT = bslmf::SelectTrait<TYPE
                                  , IsBool
                                  , IsChar
                                  , IsShort
                                  , IsLong
                                  , IsFloat>::ORDINAL,

        RESULT_A = bslmf::SelectTrait<TYPE
                                    , IsBool
                                    , IsChar>::ORDINAL,

        RESULT_B = bslmf::SelectTrait<TYPE
                                    , IsShort
                                    , IsLong
                                    , IsFloat>::ORDINAL
    };

    typedef bslmf::SelectTrait<TYPE
                             , IsBool
                             , IsChar
                             , IsShort
                             , IsLong
                             , IsFloat> Selection;

    typedef bslmf::SelectTrait<TYPE
                             , IsBool
                             , IsChar> Selection_A;

    typedef bslmf::SelectTrait<TYPE
                             , IsShort
                             , IsLong
                             , IsFloat> Selection_B;

    ASSERT(! (RESULT_A && RESULT_B));
    ASSERT(RESULT == RESULT_A ? RESULT_A : 2 + RESULT_B);

    ASSERT(RESULT   == whichTrait(Selection()));
    ASSERT(RESULT_A == whichTrait(Selection_A()));
    if (RESULT_B) {
        ASSERT(2 + RESULT_B == whichTrait(Selection_B()));
    }
    else {
        ASSERT(0 == whichTrait(Selection_B()));
    }

    return RESULT;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates the intended usage of this component.
//
///Example 1: Dispatch on traits
/// - - - - - - - - - - - - - -
// We would like to create a function template,
// 'ScalarPrimitives::copyConstruct', that takes an original object and an
// allocator constructs a copy of 'original' using the most efficient valid
// mechanism.  The function should take into account that the original type
// might be bitwise copyable, or have an allocator that can be different in
// the copy than in the original object, or that the original might be a pair
// type, where the correct method of copying 'first' and 'second' is
// (recursively) governed by the same concerns.
//
// The old (legacy) 'bsls::HasTrait' mechanism has a clumsy mechanism for
// dispatching on multiple traits at once.  For example, the
// 'bslalg::scalarprimitives::copyConstruct', function uses four different
// implementations, depending on the traits of the object being copied.  The
// existing code looks like this:
//..
//  template <class TARGET_TYPE>
//  inline
//  void
//  ScalarPrimitives::copyConstruct(TARGET_TYPE        *address,
//                                  const TARGET_TYPE&  original,
//                                  bslma::Allocator   *allocator)
//  {
//      BSLS_ASSERT_SAFE(address);
//
//      enum {
//          VALUE = HasTrait<TARGET_TYPE,
//                                  TypeTraitUsesBslmaAllocator
//                                 >::VALUE ? Imp::USES_BSLMA_ALLOCATOR_TRAITS
//                : HasTrait<TARGET_TYPE,
//                                  TypeTraitBitwiseCopyable
//                                 >::VALUE ? Imp::BITWISE_COPYABLE_TRAITS
//                : HasTrait<TARGET_TYPE,
//                                  TypeTraitPair
//                                 >::VALUE ? Imp::PAIR_TRAITS
//                : Imp::NIL_TRAITS
//      };
//      Imp::copyConstruct(address, original, allocator,
//                         (bslmf::MetaInt<VALUE>*)0);
//  }
//..
// We would like to replace the cumbersome chain of '?:' operations with a
// clean mechanism for producing one of four different types based on the
// first matching trait.
//
// First, we create three traits metafunctions to replace the three legacy
// traits used above:
//..
    template <class TYPE> struct UsesBslmaAllocator : bsl::false_type { };
    template <class TYPE> struct IsBitwiseCopyable : bsl::false_type { };
    template <class TYPE> struct IsPair : bsl::false_type { };
//..
// Note that these definitions are simplified to avoid excess dependencies; A
// proper traits definition would inherit from 'bslmf::DetectNestedTrait'
// instead of from 'bsl::false_type'.
//
// Next, we forward-declare 'bslma::Allocator' and
// 'bslalg::scalarprimitives::copyConstruct':
//..
    namespace bslma { class Allocator; }

    namespace bslalg {
    struct ScalarPrimitives {
        template <class TARGET_TYPE>
        static void copyConstruct(TARGET_TYPE        *address,
                                  const TARGET_TYPE&  original,
                                  bslma::Allocator   *allocator);
    };
//..
// Next, we implement three overloads of 'Imp::copyConstruct', each taking a
// different trait specialization. A fourth overload takes 'false_type'
// instead of a trait specialization, for those types that don't match any
// traits.  For testing purposes, in addition to copying the data member, each
// overload also increments a separate counter.  These implementations are
// slightly simplified for readability:
//..
    struct Imp {

        // Counters for counting overload calls
        static int s_noTraitsCounter;
        static int s_usesBslmaAllocatorCounter;
        static int s_isPairCounter;
        static int s_isBitwiseCopyableCounter;

        static void clearCounters() {
            s_noTraitsCounter = 0;
            s_usesBslmaAllocatorCounter = 0;
            s_isPairCounter = 0;
            s_isBitwiseCopyableCounter = 0;
        }

        template <class TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                                 *address,
                      const TARGET_TYPE&                           original,
                      bslma::Allocator                            *allocator,
                      bslmf::SelectTraitCase<UsesBslmaAllocator>)
        {
            new (address) TARGET_TYPE(original, allocator);
            ++s_usesBslmaAllocatorCounter;
        }

        template <class TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                 *address,
                      const TARGET_TYPE&           original,
                      bslma::Allocator            *allocator,
                      bslmf::SelectTraitCase<IsPair>)
        {
            ScalarPrimitives::copyConstruct(&address->first, original.first,
                                            allocator);
            ScalarPrimitives::copyConstruct(&address->second, original.second,
                                            allocator);
            ++s_isPairCounter;
        }

        template <class TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                             *address,
                      const TARGET_TYPE&                       original,
                      bslma::Allocator                        *,
                      bslmf::SelectTraitCase<IsBitwiseCopyable>)
        {
            memcpy(address, &original, sizeof(original));
            ++s_isBitwiseCopyableCounter;
        }

        template <class TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                *address,
                      const TARGET_TYPE&          original,
                      bslma::Allocator           *,
                      bslmf::SelectTraitCase<>)
        {
            new (address) TARGET_TYPE(original);
            ++s_noTraitsCounter;
        }
    };

    int Imp::s_noTraitsCounter = 0;
    int Imp::s_usesBslmaAllocatorCounter = 0;
    int Imp::s_isPairCounter = 0;
    int Imp::s_isBitwiseCopyableCounter = 0;
//..
// Then, we implement 'ScalarPrimitives::copyConstruct':
//..
    template <class TARGET_TYPE>
    inline void
    ScalarPrimitives::copyConstruct(TARGET_TYPE        *address,
                                    const TARGET_TYPE&  original,
                                    bslma::Allocator   *allocator)
    {
//..
// We use 'bslmf::SelectTrait' to declare 'Selection' as a specialization
// of the first match of the specified traits:
//..
        typedef typename bslmf::SelectTrait<TARGET_TYPE,
                                            UsesBslmaAllocator,
                                            IsBitwiseCopyable,
                                            IsPair>::Type Selection;
//..
// Now, we use 'Selection' to choose (at compile time), one of the
// 'Imp::copyConstruct' overloads defined above:
//..
        Imp::copyConstruct(address, original, allocator, Selection());
    } // end copyConstruct()

    }  // close namespace bslalg
//..
// Finally, we define three classes, associated with each of the three traits
// of interest, a fourth class associated with more than one trait (to show
// that the selection mechanism respects preference) and a fifth class that is
// not associated with any trait.
//
// The first class is associated with the 'UsesBslmaAllocator' trait:
//..
    class TypeWithAllocator {
        int               d_value;
        bslma::Allocator *d_alloc;
    public:
        TypeWithAllocator(int v = 0, bslma::Allocator *a = 0)       // IMPLICIT
            : d_value(v), d_alloc(a) { }
        TypeWithAllocator(const TypeWithAllocator& other,
                          bslma::Allocator *a = 0)
            : d_value(other.d_value), d_alloc(a) {  }

        int value() const { return d_value; }
        bslma::Allocator *allocator() const { return d_alloc; }
    };

    template <> struct UsesBslmaAllocator<TypeWithAllocator>
        : bsl::true_type { };
//..
// The second class is associated with the 'IsBitwiseCopyable' trait:
//..
    class BitwiseCopyableType {
        int d_value;
    public:
        BitwiseCopyableType(int v = 0) : d_value(v) { }             // IMPLICIT
        int value() const { return d_value; }
    };

    template <> struct IsBitwiseCopyable<BitwiseCopyableType>
        : bsl::true_type { };
//..
// The third class is associated with the 'IsPair' trait:
//..
    struct PairType {
        TypeWithAllocator   first;
        BitwiseCopyableType second;

        PairType(int a, int b) : first(a), second(b) { }
    };

    template <> struct IsPair<PairType> : bsl::true_type { };
//..
// The fourth class is associated with both the the 'IsPair' and
// 'IsBitwiseCopyable' traits:
//..
    struct BitwiseCopyablePairType {
        BitwiseCopyableType first;
        BitwiseCopyableType second;

        BitwiseCopyablePairType(int a, int b) : first(a), second(b) { }
    };

    template <> struct IsPair<BitwiseCopyablePairType> : bsl::true_type { };
    template <> struct IsBitwiseCopyable<BitwiseCopyablePairType>
        : bsl::true_type { };
//..
// The fifth class is not associated with any explicit traits:
//..
    class TypeWithNoTraits {
        int d_value;
    public:
        TypeWithNoTraits(int v = 0) : d_value(v) { }                // IMPLICIT
        int value() const { return d_value; }
    };
//..
// We use these classes to instantiate 'ScalarPrimitives::copyConstruct' and
// verify that the most efficient copy operation that is valid for each type
// is applied:
//..
    int usageExample1()
    {
        using bslalg::Imp;

        // This buffer is properly aligned and big enough to hold any of the
        // test types.
        void *buffer[4];
        char dummy[2];  // Dummy addresses

        bslma::Allocator *a1 = (bslma::Allocator*) &dummy[0];
        bslma::Allocator *a2 = (bslma::Allocator*) &dummy[1];
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'TypeWithAllocator', we expect that the copy will have the same value but a
// different allocator than the original and that the
// 'UsesBslmaAllocator' copy implementation will be called once:
//..
        Imp::clearCounters();
        TypeWithAllocator  twa(1, a1);
        TypeWithAllocator *twaptr = (TypeWithAllocator*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(twaptr, twa, a2);
        ASSERT(1 == Imp::s_usesBslmaAllocatorCounter);
        ASSERT(1 == twaptr->value());
        ASSERT(a2 == twaptr->allocator());
        twaptr->~TypeWithAllocator();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyableType', we expect that the 'IsBitwiseCopyable' copy
// implementation will be called once:
//..
        Imp::clearCounters();
        BitwiseCopyableType  bct(2);
        BitwiseCopyableType *bctptr = (BitwiseCopyableType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(bctptr, bct, a2);
        ASSERT(1 == Imp::s_isBitwiseCopyableCounter);
        ASSERT(2 == bctptr->value());
        bctptr->~BitwiseCopyableType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'PairType', we expect that the 'IsPair' copy implementation will be
// called once for the pair as whole and that the
// 'UsesBslmaAllocator' and 'IsBitwiseCopyable' implementations
// will be called for the 'first' and 'second' members, respectively:
//..
        Imp::clearCounters();
        PairType  pt(3, 4);
        PairType *ptptr = (PairType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(ptptr, pt, a2);
        ASSERT(1 == Imp::s_isPairCounter);
        ASSERT(1 == Imp::s_usesBslmaAllocatorCounter);
        ASSERT(1 == Imp::s_usesBslmaAllocatorCounter);
        ASSERT(3 == ptptr->first.value());
        ASSERT(a2 == ptptr->first.allocator());
        ASSERT(4 == ptptr->second.value());
        ptptr->~PairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyablePairType', the 'IsBitwiseCopyable' trait takes precedence
// over the 'IsPair' trait (because it appears first in the list of traits
// used to instantiate 'SelectTrait').  Therefore, we expect to see the
// 'IsBitwiseCopyable' copy implementation called once for the whole
// pair and the 'IsPair' copy implementation not called at all:
//..
        Imp::clearCounters();
        BitwiseCopyablePairType  bcpt(5, 6);
        BitwiseCopyablePairType *bcptbcptr = (BitwiseCopyablePairType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(bcptbcptr, bcpt, a2);
        // Prefer IsBitwiseCopyable over IsPair trait
        ASSERT(1 == Imp::s_isBitwiseCopyableCounter);
        ASSERT(0 == Imp::s_isPairCounter);
        ASSERT(5 == bcptbcptr->first.value());
        ASSERT(6 == bcptbcptr->second.value());
        bcptbcptr->~BitwiseCopyablePairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'TypeWithNoTraits', we expect none of the specialized copy implementations
// to be called, thus defaulting to the 'false_type' copy implementation:
//..
        Imp::clearCounters();
        TypeWithNoTraits  twnt(7);
        TypeWithNoTraits *twntptr = (TypeWithNoTraits*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(twntptr, twnt, a2);
        ASSERT(1 == Imp::s_noTraitsCounter);
        ASSERT(7 == twntptr->value());
        twntptr->~TypeWithNoTraits();

        return 0;
    }
//..
// Note that using 'SelectTraits' for dispatching using overloading imposes
// little or no overhead, since the compiler typically generates no code for
// the constructor or copy constructor of the trait argument to
// the overloaded functions.  When inlining is in effect, the result is very
// efficient.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();

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
                            "\n==============\n");

        ASSERT(breathingTest<bool>() == 1);
        ASSERT(breathingTest<char>() == 2);
        ASSERT(breathingTest<short>() == 3);
        ASSERT(breathingTest<long>() == 4);
        ASSERT(breathingTest<float>() == 5);

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
