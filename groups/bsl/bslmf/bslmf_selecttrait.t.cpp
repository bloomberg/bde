// bslmf_selecttrait.t.cpp                                            -*-C++-*-

#include "bslmf_selecttrait.h"

#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;
using namespace std;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

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
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

template <class TYPE> struct IsBool : bslmf::false_type { };
template <> struct IsBool<bool>     : bslmf::true_type { };

template <class TYPE> struct IsChar : bslmf::false_type { };
template <> struct IsChar<char>     : bslmf::true_type { };

template <class TYPE> struct IsShort : bslmf::false_type { };
template <> struct IsShort<short>    : bslmf::true_type { };

template <class TYPE> struct IsLong : bslmf::false_type { };
template <> struct IsLong<long>     : bslmf::true_type { };

template <class TYPE> struct IsFloat : bslmf::false_type { };
template <> struct IsFloat<float>    : bslmf::true_type { };

template <class TYPE>
int breathingTest()
{
    // Use enum to ensure that evaluation is at compile time
    enum {
        RESULT = bslmf::SelectTrait<TYPE
                                  , IsBool,  1
                                  , IsChar,  2
                                  , IsShort, 3
                                  , IsLong,  4
                                  , IsFloat, 5>::value,

        RESULT_A = bslmf::SelectTrait<TYPE
                                    , IsBool, 1
                                    , IsChar, 2
                                    , bslmf::SelectTraitDefault, 0>::value,

        RESULT_B = bslmf::SelectTrait<TYPE
                                    , IsShort, 3
                                    , IsLong,  4
                                    , IsFloat, 5
                                    , bslmf::SelectTraitDefault, 0>::value,
    };

    ASSERT(! (RESULT_A && RESULT_B));
    ASSERT(RESULT == RESULT_A + RESULT_B);

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
// (recursively) goverened by the same concerns.
//
// The old (legacy) 'bsls::HasTrait' mechanism has a clumsy mechanism for
// dispatching on multple traits at once.  For example, the
// 'bslalg::scalarprimitives::copyConstruct', function uses four different
// implementations, depending on the traits of the object being copied.  The
// existing code looks like this:
//..
//  template <typename TARGET_TYPE>
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
    template <class TYPE> struct UsesBslmaAllocator : bslmf::false_type { };
    template <class TYPE> struct IsBitwiseCopyable : bslmf::false_type { };
    template <class TYPE> struct IsPair : bslmf::false_type { };
//..
// Note that these definitions are simplified to avoid excess dependencies; A
// proper traits definition would inherit from 'bslmf::DetectNestedTrait'
// instead of from 'bslmf::false_type'.
//
// Next, we forward-declare 'bslma::Allocator' and
// 'bslalg::scalarprimitives::copyConstruct':
//..
    namespace bslma { class Allocator; }

    namespace bslalg {
    struct ScalarPrimitives {
        template <typename TARGET_TYPE>
        static void copyConstruct(TARGET_TYPE        *address,
                                  const TARGET_TYPE&  original,
                                  bslma::Allocator   *allocator);
    };
//..
// Next, we implement four overloads of 'Imp::copyConstruct', each taking a
// different 'integer_constant' specialization.  For testing purposes, in
// addition to copying the data member, each overload also increments a
// separate counter.  These implemenations are slightly simplified for
// readability:
//..
    struct Imp {

        enum {
            // These constants are used in the overloads below, when the last
            // argument is of type 'bslmf::MetaInt<N> *', indicating that
            // 'TARGET_TYPE' has the traits for which the enumerator equal to
            // 'N' is named.

            USES_BSLMA_ALLOCATOR_TRAITS     = 5,
            PAIR_TRAITS                     = 4,
            BITWISE_COPYABLE_TRAITS         = 2,
            NIL_TRAITS                      = 0
        };

        static int d_counters[USES_BSLMA_ALLOCATOR_TRAITS + 1];

        static void clearCounters()
            { std::memset(d_counters, 0, sizeof(d_counters)); }

        template <typename TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                                 *address,
                      const TARGET_TYPE&                           original,
                      bslma::Allocator                            *allocator,
                      integer_constant<int, USES_BSLMA_ALLOCATOR_TRAITS>)
        {
            new (address) TARGET_TYPE(original, allocator);
            ++d_counters[USES_BSLMA_ALLOCATOR_TRAITS];
        }

        template <typename TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                 *address,
                      const TARGET_TYPE&           original,
                      bslma::Allocator            *allocator,
                      integer_constant<int, PAIR_TRAITS>)
        {
            ScalarPrimitives::copyConstruct(&address->first, original.first,
                                            allocator);
            ScalarPrimitives::copyConstruct(&address->second, original.second,
                                            allocator);
            ++d_counters[PAIR_TRAITS];
        }

        template <typename TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                             *address,
                      const TARGET_TYPE&                       original,
                      bslma::Allocator                        *,
                      integer_constant<int, BITWISE_COPYABLE_TRAITS>)
        {
            std::memcpy(address, &original, sizeof(original));
            ++d_counters[BITWISE_COPYABLE_TRAITS];
        }

        template <typename TARGET_TYPE>
        static void
        copyConstruct(TARGET_TYPE                *address,
                      const TARGET_TYPE&          original,
                      bslma::Allocator           *,
                      integer_constant<int, NIL_TRAITS>)
        {
            new (address) TARGET_TYPE(original);
            ++d_counters[NIL_TRAITS];
        }
    };

    int bslalg::Imp::d_counters[USES_BSLMA_ALLOCATOR_TRAITS + 1] = { 0 };
//..
// Then, we implement 'ScalarPrimitives::copyConstruct':
//..
    template <typename TARGET_TYPE>
    inline void
    ScalarPrimitives::copyConstruct(TARGET_TYPE        *address,
                                    const TARGET_TYPE&  original,
                                    bslma::Allocator   *allocator)
    {
//..
// We use 'bslmf::SelectTrait' to declare 'Selection' as an instantiation of
// 'integer_constant' corresponding to the first match of the specified
// traits:
//..
        typedef bslmf::SelectTrait<TARGET_TYPE,
            UsesBslmaAllocator,        Imp::USES_BSLMA_ALLOCATOR_TRAITS,
            IsBitwiseCopyable,         Imp::BITWISE_COPYABLE_TRAITS,
            IsPair,                    Imp::PAIR_TRAITS,
            bslmf::SelectTraitDefault, Imp::NIL_TRAITS> Selection;
//..
// Now, we use 'Selection' to choose (at compiler time), one of the
// 'Imp::copyConstruct' overloads defined above:
//..
        Imp::copyConstruct(address, original, allocator, Selection());
    }
    } // Close namespace bslalg
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
        TypeWithAllocator(int v = 0, bslma::Allocator *a = 0)
            : d_value(v), d_alloc(a) { }
        TypeWithAllocator(const TypeWithAllocator& other,
                          bslma::Allocator *a = 0)
            : d_value(other.d_value), d_alloc(a) {  }

        int value() const { return d_value; }
        bslma::Allocator *allocator() const { return d_alloc; }
    };

    template <> struct UsesBslmaAllocator<TypeWithAllocator>
        : bslmf::true_type { };
//..
// The second class is associated with the 'IsBitwiseCopyiable' trait:
//..
    class BitwiseCopyableType {
        int d_value;
    public:
        BitwiseCopyableType(int v = 0) : d_value(v) { }
        int value() const { return d_value; }
    };

    template <> struct IsBitwiseCopyable<BitwiseCopyableType>
        : bslmf::true_type { };
//..
// The third class is associated with the 'IsPair' trait:
//..
    struct PairType {
        TypeWithAllocator   first;
        BitwiseCopyableType second;

        PairType(int a, int b) : first(a), second(b) { }
    };

    template <> struct IsPair<PairType> : bslmf::true_type { };
//..
// The fourth class is associated with both the the 'IsPair' and
// 'IsBitwiseCopyiable' traits:
//..
    struct BitwiseCopyablePairType {
        BitwiseCopyableType first;
        BitwiseCopyableType second;

        BitwiseCopyablePairType(int a, int b) : first(a), second(b) { }
    };

    template <> struct IsPair<BitwiseCopyablePairType> : bslmf::true_type { };
    template <> struct IsBitwiseCopyable<BitwiseCopyablePairType>
        : bslmf::true_type { };
//..
// The fifth class is not associated with any explicit traits:
//..
    class TypeWithNoTraits {
        int d_value;
    public:
        TypeWithNoTraits(int v = 0) : d_value(v) { }
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
// 'USES_BSLMA_ALLOCATOR_TRAITS' copy implementation will be called once:
//..
        Imp::clearCounters();
        TypeWithAllocator  twa(1, a1);
        TypeWithAllocator *twaptr = (TypeWithAllocator*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(twaptr, twa, a2);
        ASSERT(1 == Imp::d_counters[Imp::USES_BSLMA_ALLOCATOR_TRAITS]);
        ASSERT(1 == twaptr->value());
        ASSERT(a2 == twaptr->allocator());
        twaptr->~TypeWithAllocator();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyableType', we expect that the 'BITWISE_COPYABLE_TRAITS' copy
// implementation will be called once:
//..
        Imp::clearCounters();
        BitwiseCopyableType  bct(2);
        BitwiseCopyableType *bctptr = (BitwiseCopyableType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(bctptr, bct, a2);
        ASSERT(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
        ASSERT(2 == bctptr->value());
        bctptr->~BitwiseCopyableType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'PairType', we expect that the 'PAIR_TRAITS' copy implementation will be
// called once for the pair as whole and that the
// 'USES_BSLMA_ALLOCATOR_TRAITS' and 'BITWISE_COPYABLE_TRAITS' implementations
// will be called for the 'first' and 'second' members, respectively:
//..
        Imp::clearCounters();
        PairType  pt(3, 4);
        PairType *ptptr = (PairType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(ptptr, pt, a2);
        ASSERT(1 == Imp::d_counters[Imp::PAIR_TRAITS]);
        ASSERT(1 == Imp::d_counters[Imp::USES_BSLMA_ALLOCATOR_TRAITS]);
        ASSERT(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
        ASSERT(3 == ptptr->first.value());
        ASSERT(a2 == ptptr->first.allocator());
        ASSERT(4 == ptptr->second.value());
        ptptr->~PairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyablePairType', the 'IsBitwiseCopyable' trait takes precedence
// over the 'IsPair' trait (because it appears first in the list of traits
// used to instantiate 'SelectTrait').  Therefore, we expect to see the
// 'BITWISE_COPYABLE_TRAITS' copy implementation called once for the whole
// pair and the 'PAIR_TRAITS' copy implementation not called at all:
//..
        Imp::clearCounters();
        BitwiseCopyablePairType  bcpt(5, 6);
        BitwiseCopyablePairType *bcptbcptr = (BitwiseCopyablePairType*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(bcptbcptr, bcpt, a2);
        // Prefer IsBitwiseCopyable over IsPair trait
        ASSERT(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
        ASSERT(0 == Imp::d_counters[Imp::PAIR_TRAITS]);
        ASSERT(5 == bcptbcptr->first.value());
        ASSERT(6 == bcptbcptr->second.value());
        bcptbcptr->~BitwiseCopyablePairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'TypeWithNoTraits', we expect none of the specialized copy implementations
// to be called, thus defaulting to the 'NIL_TRAITS' copy implementation:
//..
        Imp::clearCounters();
        TypeWithNoTraits  twnt(7);
        TypeWithNoTraits *twntptr = (TypeWithNoTraits*) buffer;
        bslalg::ScalarPrimitives::copyConstruct(twntptr, twnt, a2);
        ASSERT(1 == Imp::d_counters[Imp::NIL_TRAITS]);
        ASSERT(7 == twntptr->value());
        twntptr->~TypeWithNoTraits();

        return 0;
    }
//..
// Note that using 'SelectTraits' for dispatching using overloading imposes
// little or no overhead, since the compiler typically generates no code for
// the constructor or copy constructor of the 'integer_constant' argument to
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
