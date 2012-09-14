// bslmf_selecttrait.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_SELECTTRAIT
#define INCLUDED_BSLMF_SELECTTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
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
//..
//  template <typename TARGET_TYPE>
//  inline
//  void
//  ScalarPrimitives::copyConstruct(TARGET_TYPE        *address,
//                                  const TARGET_TYPE&  original,
//                                  bslma::Allocator   *allocator)
//  {
//      BSLS_assert_SAFE(address);
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
//..
// We would like to replace the cumbersome chain of '?:' operations with a
// clean mechanism for producing one of four different types based on the
// first matching trait.
//
// First, we create three traits metafunctions to replace the three legacy
// traits used above:
//..
//  template <class TYPE> struct UsesBslmaAllocator : bsl::false_type { };
//  template <class TYPE> struct IsBitwiseCopyable : bsl::false_type { };
//  template <class TYPE> struct IsPair : bsl::false_type { };
//..
// Note that these definitions are simplified to avoid excess dependencies; A
// proper traits definition would inherit from 'bslmf::DetectNestedTrait'
// instead of from 'bsl::false_type'.
//
// Next, we forward-declare 'bslma::Allocator' and
// 'bslalg::scalarprimitives::copyConstruct':
//..
//  namespace bslma { class Allocator; }
//
//  namespace bslalg {
//  struct ScalarPrimitives {
//      template <typename TARGET_TYPE>
//      static void copyConstruct(TARGET_TYPE        *address,
//                                const TARGET_TYPE&  original,
//                                bslma::Allocator   *allocator);
//  };
//..
// Next, we implement four overloads of 'Imp::copyConstruct', each taking a
// different 'bsl::integer_constant' specialization.  For testing purposes, in
// addition to copying the data member, each overload also increments a
// separate counter.  These implemenations are slightly simplified for
// readability:
//..
//  struct Imp {
//
//      enum {
//          // These constants are used in the overloads below, when the last
//          // argument is of type 'bslmf::MetaInt<N> *', indicating that
//          // 'TARGET_TYPE' has the traits for which the enumerator equal to
//          // 'N' is named.
//
//          USES_BSLMA_ALLOCATOR_TRAITS     = 5,
//          PAIR_TRAITS                     = 4,
//          BITWISE_COPYABLE_TRAITS         = 2,
//          NIL_TRAITS                      = 0
//      };
//
//      static int d_counters[USES_BSLMA_ALLOCATOR_TRAITS + 1];
//
//      static void clearCounters()
//          { std::memset(d_counters, 0, sizeof(d_counters)); }
//
//      template <typename TARGET_TYPE>
//      static void
//      copyConstruct(TARGET_TYPE                                 *address,
//                    const TARGET_TYPE&                           original,
//                    bslma::Allocator                            *allocator,
//                    bsl::integer_constant<int, USES_BSLMA_ALLOCATOR_TRAITS>)
//      {
//          new (address) TARGET_TYPE(original, allocator);
//          ++d_counters[USES_BSLMA_ALLOCATOR_TRAITS];
//      }
//
//      template <typename TARGET_TYPE>
//      static void
//      copyConstruct(TARGET_TYPE                 *address,
//                    const TARGET_TYPE&           original,
//                    bslma::Allocator            *allocator,
//                    bsl::integer_constant<int, PAIR_TRAITS>)
//      {
//          ScalarPrimitives::copyConstruct(&address->first, original.first,
//                                          allocator);
//          ScalarPrimitives::copyConstruct(&address->second, original.second,
//                                          allocator);
//          ++d_counters[PAIR_TRAITS];
//      }
//
//      template <typename TARGET_TYPE>
//      static void
//      copyConstruct(TARGET_TYPE                             *address,
//                    const TARGET_TYPE&                       original,
//                    bslma::Allocator                        *,
//                    bsl::integer_constant<int, BITWISE_COPYABLE_TRAITS>)
//      {
//          std::memcpy(address, &original, sizeof(original));
//          ++d_counters[BITWISE_COPYABLE_TRAITS];
//      }
//
//      template <typename TARGET_TYPE>
//      static void
//      copyConstruct(TARGET_TYPE                *address,
//                    const TARGET_TYPE&          original,
//                    bslma::Allocator           *,
//                    bsl::integer_constant<int, NIL_TRAITS>)
//      {
//          new (address) TARGET_TYPE(original);
//          ++d_counters[NIL_TRAITS];
//      }
//  };
//
//  int bslalg::Imp::d_counters[USES_BSLMA_ALLOCATOR_TRAITS + 1] = { 0 };
//..
// Then, we implement 'ScalarPrimitives::copyConstruct':
//..
//  template <typename TARGET_TYPE>
//  inline void
//  ScalarPrimitives::copyConstruct(TARGET_TYPE        *address,
//                                  const TARGET_TYPE&  original,
//                                  bslma::Allocator   *allocator)
//  {
//..
// We use 'bslmf::SelectTrait' to declare 'Selection' as an instantiation of
// 'bsl::integer_constant' corresponding to the first match of the specified
// traits:
//..
//      typedef bslmf::SelectTrait<TARGET_TYPE,
//          UsesBslmaAllocator,        Imp::USES_BSLMA_ALLOCATOR_TRAITS,
//          IsBitwiseCopyable,         Imp::BITWISE_COPYABLE_TRAITS,
//          IsPair,                    Imp::PAIR_TRAITS,
//          bslmf::SelectTraitDefault, Imp::NIL_TRAITS> Selection;
//..
// Now, we use 'Selection' to choose (at compiler time), one of the
// 'Imp::copyConstruct' overloads defined above:
//..
//      Imp::copyConstruct(address, original, allocator, Selection());
//  }
//  } // Close namespace bslalg
//..
// Finally, we define three classes, associated with each of the three traits
// of interest, a fourth class associated with more than one trait (to show
// that the selection mechanism respects preference) and a fifth class that is
// not associated with any trait.
//
// The first class is associated with the 'UsesBslmaAllocator' trait:
//..
//  class TypeWithAllocator {
//      int               d_value;
//      bslma::Allocator *d_alloc;
//  public:
//      TypeWithAllocator(int v = 0, bslma::Allocator *a = 0)
//          : d_value(v), d_alloc(a) { }
//      TypeWithAllocator(const TypeWithAllocator& other,
//                        bslma::Allocator *a = 0)
//          : d_value(other.d_value), d_alloc(a) {  }
//
//      int value() const { return d_value; }
//      bslma::Allocator *allocator() const { return d_alloc; }
//  };
//
//  template <> struct UsesBslmaAllocator<TypeWithAllocator>
//      : bsl::true_type { };
//..
// The second class is associated with the 'IsBitwiseCopyiable' trait:
//..
//  class BitwiseCopyableType {
//      int d_value;
//  public:
//      BitwiseCopyableType(int v = 0) : d_value(v) { }
//      int value() const { return d_value; }
//  };
//
//  template <> struct IsBitwiseCopyable<BitwiseCopyableType>
//      : bsl::true_type { };
//..
// The third class is associated with the 'IsPair' trait:
//..
//  struct PairType {
//      TypeWithAllocator   first;
//      BitwiseCopyableType second;
//
//      PairType(int a, int b) : first(a), second(b) { }
//  };
//
//  template <> struct IsPair<PairType> : bsl::true_type { };
//..
// The fourth class is associated with both the the 'IsPair' and
// 'IsBitwiseCopyiable' traits:
//..
//  struct BitwiseCopyablePairType {
//      BitwiseCopyableType first;
//      BitwiseCopyableType second;
//
//      BitwiseCopyablePairType(int a, int b) : first(a), second(b) { }
//  };
//
//  template <> struct IsPair<BitwiseCopyablePairType> : bsl::true_type { };
//  template <> struct IsBitwiseCopyable<BitwiseCopyablePairType>
//      : bsl::true_type { };
//..
// The fifth class is not associated with any explicit traits:
//..
//  class TypeWithNoTraits {
//      int d_value;
//  public:
//      TypeWithNoTraits(int v = 0) : d_value(v) { }
//      int value() const { return d_value; }
//  };
//
//..
// We use these classes to instantiate 'ScalarPrimitives::copyConstruct' and
// verify that the most efficient copy operation that is valid for each type
// is applied:
//..
//  int main()
//  {
//      using bslalg::Imp;
//
//      // This buffer is properly aligned and big enough to hold any of the
//      // test types.
//      void *buffer[4];
//      char dummy[2];  // Dummy addresses
//
//      bslma::Allocator *a1 = (bslma::Allocator*) &dummy[0];
//      bslma::Allocator *a2 = (bslma::Allocator*) &dummy[1];
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'TypeWithAllocator', we expect that the copy will have the same value but a
// different allocator than the original and that the
// 'USES_BSLMA_ALLOCATOR_TRAITS' copy implementation will be called once:
//..
//      Imp::clearCounters();
//      TypeWithAllocator  twa(1, a1);
//      TypeWithAllocator *twaptr = (TypeWithAllocator*) buffer;
//      bslalg::ScalarPrimitives::copyConstruct(twaptr, twa, a2);
//      assert(1 == Imp::d_counters[Imp::USES_BSLMA_ALLOCATOR_TRAITS]);
//      assert(1 == twaptr->value());
//      assert(a2 == twaptr->allocator());
//      twaptr->~TypeWithAllocator();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyableType', we expect that the 'BITWISE_COPYABLE_TRAITS' copy
// implementation will be called once:
//..
//      Imp::clearCounters();
//      BitwiseCopyableType  bct(2);
//      BitwiseCopyableType *bctptr = (BitwiseCopyableType*) buffer;
//      bslalg::ScalarPrimitives::copyConstruct(bctptr, bct, a2);
//      assert(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
//      assert(2 == bctptr->value());
//      bctptr->~BitwiseCopyableType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'PairType', we expect that the 'PAIR_TRAITS' copy implementation will be
// called once for the pair as whole and that the
// 'USES_BSLMA_ALLOCATOR_TRAITS' and 'BITWISE_COPYABLE_TRAITS' implementations
// will be called for the 'first' and 'second' members, respectively:
//..
//      Imp::clearCounters();
//      PairType  pt(3, 4);
//      PairType *ptptr = (PairType*) buffer;
//      bslalg::ScalarPrimitives::copyConstruct(ptptr, pt, a2);
//      assert(1 == Imp::d_counters[Imp::PAIR_TRAITS]);
//      assert(1 == Imp::d_counters[Imp::USES_BSLMA_ALLOCATOR_TRAITS]);
//      assert(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
//      assert(3 == ptptr->first.value());
//      assert(a2 == ptptr->first.allocator());
//      assert(4 == ptptr->second.value());
//      ptptr->~PairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'BitwiseCopyablePairType', the 'IsBitwiseCopyable' trait takes precedence
// over the 'IsPair' trait (because it appears first in the list of traits
// used to instantiate 'SelectTrait').  Therefore, we expect to see the
// 'BITWISE_COPYABLE_TRAITS' copy implementation called once for the whole
// pair and the 'PAIR_TRAITS' copy implementation not called at all:
//..
//      Imp::clearCounters();
//      BitwiseCopyablePairType  bcpt(5, 6);
//      BitwiseCopyablePairType *bcptbcptr = (BitwiseCopyablePairType*) buffer;
//      bslalg::ScalarPrimitives::copyConstruct(bcptbcptr, bcpt, a2);
//      // Prefer IsBitwiseCopyable over IsPair trait
//      assert(1 == Imp::d_counters[Imp::BITWISE_COPYABLE_TRAITS]);
//      assert(0 == Imp::d_counters[Imp::PAIR_TRAITS]);
//      assert(5 == bcptbcptr->first.value());
//      assert(6 == bcptbcptr->second.value());
//      bcptbcptr->~BitwiseCopyablePairType();
//..
// When we call 'ScalarPrimitives::copyConstruct' for an object of
// 'TypeWithNoTraits', we expect none of the specialized copy implementations
// to be called, thus defaulting to the 'NIL_TRAITS' copy implementation:
//..
//      Imp::clearCounters();
//      TypeWithNoTraits  twnt(7);
//      TypeWithNoTraits *twntptr = (TypeWithNoTraits*) buffer;
//      bslalg::ScalarPrimitives::copyConstruct(twntptr, twnt, a2);
//      assert(1 == Imp::d_counters[Imp::NIL_TRAITS]);
//      assert(7 == twntptr->value());
//      twntptr->~TypeWithNoTraits();
//
//      return 0;
//  }
//..
// Note that using 'SelectTraits' for dispatching using overloading imposes
// little or no overhead, since the compiler typically generates no code for
// the constructor or copy constructor of the 'bsl::integer_constant' argument
// to the overloaded functions.  When inlining is in effect, the result is very
// efficient.

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_SWITCH
#include <bslmf_switch.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // ========================
                        // struct SelectTrait_False
                        // ========================

template <class TYPE>
struct SelectTrait_False : bsl::false_type
{
    // Metafunction that always returns false.
};

                        // ======================
                        // struct SelectTrait_Imp
                        // ======================

template <class TRAIT_EVAL1, class TRAIT_EVAL2, class TRAIT_EVAL3,
          class TRAIT_EVAL4, class TRAIT_EVAL5, class TRAIT_EVAL6,
          class TRAIT_EVAL7, class TRAIT_EVAL8, class TRAIT_EVAL9>
struct SelectTrait_Imp
{
    enum { ORDINAL = (TRAIT_EVAL1::value ? 1 :
                      TRAIT_EVAL2::value ? 2 :
                      TRAIT_EVAL3::value ? 3 :
                      TRAIT_EVAL4::value ? 4 :
                      TRAIT_EVAL5::value ? 5 :
                      TRAIT_EVAL6::value ? 6 :
                      TRAIT_EVAL7::value ? 7 :
                      TRAIT_EVAL8::value ? 8 :
                      TRAIT_EVAL9::value ? 9 : 0) };

    typedef typename Switch<ORDINAL, bsl::false_type,
                            TRAIT_EVAL1, TRAIT_EVAL2, TRAIT_EVAL3, TRAIT_EVAL4,
                            TRAIT_EVAL5, TRAIT_EVAL6, TRAIT_EVAL7, TRAIT_EVAL8,
                            TRAIT_EVAL9>::Type Type;

};

                        // ==================
                        // struct SelectTrait
                        // ==================

template <class TYPE, 
          template <class T> class TRAIT1,
          template <class T> class TRAIT2 = SelectTrait_False,
          template <class T> class TRAIT3 = SelectTrait_False,
          template <class T> class TRAIT4 = SelectTrait_False,
          template <class T> class TRAIT5 = SelectTrait_False,
          template <class T> class TRAIT6 = SelectTrait_False,
          template <class T> class TRAIT7 = SelectTrait_False,
          template <class T> class TRAIT8 = SelectTrait_False,
          template <class T> class TRAIT9 = SelectTrait_False>
struct SelectTrait : SelectTrait_Imp<TRAIT1<TYPE>, TRAIT2<TYPE>, TRAIT3<TYPE>,
                                     TRAIT4<TYPE>, TRAIT5<TYPE>, TRAIT6<TYPE>,
                                     TRAIT7<TYPE>, TRAIT8<TYPE>, TRAIT9<TYPE>
                                    >::Type
{
private:
    typedef SelectTrait_Imp<TRAIT1<TYPE>, TRAIT2<TYPE>, TRAIT3<TYPE>,
                            TRAIT4<TYPE>, TRAIT5<TYPE>, TRAIT6<TYPE>,
                            TRAIT7<TYPE>, TRAIT8<TYPE>, TRAIT9<TYPE> > Imp;

public:
    typedef typename Imp::Type Type;

    static const int ORDINAL = Imp::ORDINAL;

    // Class description
    typedef bsl::integer_constant<int, ORDINAL> OrdinalType;
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_SELECTTRAIT)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
