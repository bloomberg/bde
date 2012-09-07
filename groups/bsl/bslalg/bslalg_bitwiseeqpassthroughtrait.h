// bslalg_bitwiseeqpassthroughtrait.h                                 -*-C++-*-
#ifndef INCLUDED_BSLALG_BITWISEEQPASSTHROUGHTRAIT
#define INCLUDED_BSLALG_BITWISEEQPASSTHROUGHTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for computing compile-time traits.
//
//@CLASSES:
//  bslalg::BitwiseEqPassthroughTrait: pass-through trait mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslalg::BitwiseEqPassthroughTrait', that allows computation of the
// 'bslalg::TypeTraitBitwiseEqualityComparable' trait for classes knowing the
// types of its members.
//
///Usage
///-----
// This class simplifies the declaration of
// 'bslalg::TypeTraitBitwiseEqualityComparable' trait for a class with multiple
// data members.  This is especially useful for attribute classes.  For
// example, suppose we have an unconstrained attribute class 'my_Point':
//..
//  class my_Point {
//
//      // DATA
//      int d_x;
//      int d_y;
//
//    public:
//      BSLALG_DECLARE_NESTED_TRAITS(
//                                 my_Point,
//                                 bslalg::TypeTraitBitwiseEqualityComparable);
//
//      // ...
//  };
//..
// Note that the methods of the 'my_Point' class is elided as they are not
// relevant to this usage example.  Then we have another attribute class
// 'ColorPoint' where color is a 32-bit number.
//..
//  class my_ColorPoint {
//
//      // PRIVATE TYPES
//      typedef bslalg::BitwiseEqPassthroughTrait<my_Point, int>::Type BEC;
//
//      // DATA
//      my_Point d_point;
//      int      d_color;
//
//    public:
//      BSLALG_DECLARE_NESTED_TRAITS(my_ColorPoint, BEC);
//
//      // ...
//  };
//..
// Instead of having to check whether 'my_Point' is bitwise equality
// comparable, and whether the combination of 'Point' and 'int' inside the
// class results in a bitwise equality comparable class, the use of
// 'bslalg::BitwiseEqPassthroughTrait' encapsulates all the meta-programming
// check.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

namespace BloombergLP {

namespace bslalg {

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct BitwiseEqPassthroughTrait_PackedStruct;

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp;

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
class BitwiseEqPassthroughTrait_Imp;

                   // ===============================
                   // struct BitwiseEqPasstroughTrait
                   // ===============================

template <typename A1 = bslmf::Nil, typename A2 = bslmf::Nil,
          typename A3 = bslmf::Nil, typename A4 = bslmf::Nil,
          typename A5 = bslmf::Nil, typename A6 = bslmf::Nil,
          typename A7 = bslmf::Nil, typename A8 = bslmf::Nil,
          typename A9 = bslmf::Nil>
struct BitwiseEqPassthroughTrait : BitwiseEqPassthroughTrait_Imp<
                                    A1, A2, A3, A4, A5, A6, A7, A8, A9>::Type {
    // If the parameterized 'A1' up to 'A9' types have the
    // 'TypeTraitBitwiseEqualityComparable' trait, and if a structure packed
    // with these types (excluding the non-specified types) has the same size
    // as the sum of the sizes of each type (i.e., there is no packing), then
    // evaluate to 'TypeTraitBitwiseEqualityComparable', else evaluate to a
    // unique class that is not this trait.  Users of this meta-function do not
    // need to expand the result with '::Type' (though they may).

    typedef typename BitwiseEqPassthroughTrait_Imp<
                                A1, A2, A3, A4, A5, A6, A7, A8, A9>::Type Type;
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

              // =========================================
              // struct BitwiseEqPassthroughTrait_NotTrait
              // =========================================

template <typename TRAIT> struct BitwiseEqPassthroughTrait_NotTrait {
    // Private class: Given a trait, this template produces a unique type which
    // is NOT the trait type and is not a trait at all.
};

               // ====================================
               // struct PassthroughTrait_PackedStruct
               // ====================================

template <typename A1, typename A2 = bslmf::Nil, typename A3 = bslmf::Nil,
                       typename A4 = bslmf::Nil, typename A5 = bslmf::Nil,
                       typename A6 = bslmf::Nil, typename A7 = bslmf::Nil,
                       typename A8 = bslmf::Nil, typename A9 = bslmf::Nil>
struct BitwiseEqPassthroughTrait_PackedStruct :
                        BitwiseEqPassthroughTrait_PackedStruct_Imp<
                                                  A1,A2,A3,A4,A5,A6,A7,A8,A9> {
    // This 'struct' is never created, in particular the types 'A1' up to 'A9'
    // do not need to have a default constructor.  Its sole purpose is to
    // compute its size.

};

              // ========================================
              // struct PassthroughTrait_PackedStruct_Imp
              // ========================================

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
    A6 d_a6;
    A7 d_a7;
    A8 d_a8;
    A9 d_a9;
};

// Specializations
template <typename A1, typename A2>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2,
        bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil,
        bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
};

template <typename A1, typename A2, typename A3>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3,
      bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
};

template <typename A1, typename A2, typename A3, typename A4>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4,
                  bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                              bslmf::Nil, bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                      A6, bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
    A6 d_a6;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6, typename A7>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                              A6, A7, bslmf::Nil, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
    A6 d_a6;
    A7 d_a7;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6, typename A7, typename A8>
struct BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                                      A6, A7, A8, bslmf::Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
    A6 d_a6;
    A7 d_a7;
    A8 d_a8;
};

                // ====================================
                // struct BitwiseEqPassthroughTrait_Imp
                // ====================================

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
class BitwiseEqPassthroughTrait_Imp {
    // Private implementation of 'BitwiseEqPassthroughTrait' class.  This class
    // computes whether the combination of parameterized types 'A1 ... A9'
    // result in a bitwise comparable type.

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;

    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    HasTrait<A5, TRAIT>::VALUE &&
                    HasTrait<A6, TRAIT>::VALUE &&
                    HasTrait<A7, TRAIT>::VALUE &&
                    HasTrait<A8, TRAIT>::VALUE &&
                    HasTrait<A9, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) + sizeof(A8) +
                    sizeof(A9) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                   A1,A2,A3,A4,A5,A6,A7,A8,A9>)
    };

  public:
    typedef typename bslmf::If<HAS_TRAIT,
                               TRAIT,
                               BitwiseEqPassthroughTrait_NotTrait<TRAIT>
                              >::Type Type;
};

// Specializations
template <typename A1>
class BitwiseEqPassthroughTrait_Imp<A1, bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                        bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                        bslmf::Nil, bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;

  public:
    typedef typename bslmf::If<
                 HasTrait<A1, TRAIT>::VALUE,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2>
class BitwiseEqPassthroughTrait_Imp<A1, A2, bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                            bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                            bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                                       A1, A2>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3,
                                    bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                    bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                                   A1, A2, A3>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4,
                                    bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                    bslmf::Nil, bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4 )==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                               A1, A2, A3, A4>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5,
                                    bslmf::Nil, bslmf::Nil, bslmf::Nil,
                                    bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    HasTrait<A5, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                           A1, A2, A3, A4, A5>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6,
                                    bslmf::Nil, bslmf::Nil, bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    HasTrait<A5, TRAIT>::VALUE &&
                    HasTrait<A6, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                       A1, A2, A3, A4, A5, A6>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6, A7,
                                    bslmf::Nil, bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    HasTrait<A5, TRAIT>::VALUE &&
                    HasTrait<A6, TRAIT>::VALUE &&
                    HasTrait<A7, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                                   A1, A2, A3, A4, A5, A6, A7>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8>
class BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6, A7, A8,
                                    bslmf::Nil> {

    // PRIVATE TYPES
    typedef TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = HasTrait<A1, TRAIT>::VALUE &&
                    HasTrait<A2, TRAIT>::VALUE &&
                    HasTrait<A3, TRAIT>::VALUE &&
                    HasTrait<A4, TRAIT>::VALUE &&
                    HasTrait<A5, TRAIT>::VALUE &&
                    HasTrait<A6, TRAIT>::VALUE &&
                    HasTrait<A7, TRAIT>::VALUE &&
                    HasTrait<A8, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) + sizeof(A8) ==
                       sizeof(BitwiseEqPassthroughTrait_PackedStruct<
                                               A1, A2, A3, A4, A5, A6, A7, A8>)
    };

  public:
    typedef typename bslmf::If<
                 HAS_TRAIT,
                 TRAIT,
                 BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

}  // close package namespace

#if defined(BDE_BACKWARD_COMPATIBILITY) && 1 == BDE_BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_BitwiseEqPassthroughTrait
#undef bslalg_BitwiseEqPassthroughTrait
#endif
#define bslalg_BitwiseEqPassthroughTrait bslalg::BitwiseEqPassthroughTrait
    // This alias is defined for backward compatibility.
#endif // BDE_BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
