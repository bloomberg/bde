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
//  bslalg_BitwiseEqPassthroughTrait: pass-through trait mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslalg_BitwiseEqPassthroughTrait', that allows computation of the
// 'bslalg_TypeTraitBitwiseEqualityComparable' trait for classes knowing the
// types of its members.
//
///Usage
///-----
// This class simplifies the declaration of
// 'bslalg_TypeTraitBitwiseEqualityComparable' trait for a class with multiple
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
//      BSLALG_DECLARE_NESTED_TRAITS(my_Point,
//                                  bslalg_TypeTraitBitwiseEqualityComparable);
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
//      typedef bslalg_BitwiseEqPassthroughTrait<my_Point, int>::Type BEC;
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
// 'bslalg_BitwiseEqPassthroughTrait' encapsulates all the meta-programming
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

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct;

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp;

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
class bslalg_BitwiseEqPassthroughTrait_Imp;

                   // ======================================
                   // struct bslalg_BitwiseEqPasstroughTrait
                   // ======================================

template <typename A1 = bslmf_Nil, typename A2 = bslmf_Nil,
          typename A3 = bslmf_Nil, typename A4 = bslmf_Nil,
          typename A5 = bslmf_Nil, typename A6 = bslmf_Nil,
          typename A7 = bslmf_Nil, typename A8 = bslmf_Nil,
          typename A9 = bslmf_Nil>
struct bslalg_BitwiseEqPassthroughTrait : bslalg_BitwiseEqPassthroughTrait_Imp<
                                    A1, A2, A3, A4, A5, A6, A7, A8, A9>::Type {
    // If the parameterized 'A1' up to 'A9' types have the
    // 'bslalg_TypeTraitBitwiseEqualityComparable' trait, and if a structure
    // packed with these types (excluding the non-specified types) has the same
    // size as the sum of the sizes of each type (i.e., there is no packing),
    // then evaluate to 'bslalg_TypeTraitBitwiseEqualityComparable', else
    // evaluate to a unique class that is not this trait.  Users of this
    // meta-function do not need to expand the result with '::Type' (though
    // they may).

    typedef typename bslalg_BitwiseEqPassthroughTrait_Imp<
                                A1, A2, A3, A4, A5, A6, A7, A8, A9>::Type Type;
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

              // ================================================
              // struct bslalg_BitwiseEqPassthroughTrait_NotTrait
              // ================================================

template <typename TRAIT> struct bslalg_BitwiseEqPassthroughTrait_NotTrait {
    // Private class: Given a trait, this template produces a unique type
    // which is NOT the trait type and is not a trait at all.
};

               // ===========================================
               // struct bslalg_PassthroughTrait_PackedStruct
               // ===========================================

template <typename A1, typename A2 = bslmf_Nil, typename A3 = bslmf_Nil,
                       typename A4 = bslmf_Nil, typename A5 = bslmf_Nil,
                       typename A6 = bslmf_Nil, typename A7 = bslmf_Nil,
                       typename A8 = bslmf_Nil, typename A9 = bslmf_Nil>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct :
                        bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<
                                                  A1,A2,A3,A4,A5,A6,A7,A8,A9> {
    // This 'struct' is never created, in particular the types 'A1' up to
    // 'A9' do not need to have a default constructor.  Its sole purpose is to
    // compute its size.

};

              // ===============================================
              // struct bslalg_PassthroughTrait_PackedStruct_Imp
              // ===============================================

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp {

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
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2,
 bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
};

template <typename A1, typename A2, typename A3>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3,
            bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
};

template <typename A1, typename A2, typename A3, typename A4>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4,
                       bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                  bslmf_Nil, bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // DATA
    A1 d_a1;
    A2 d_a2;
    A3 d_a3;
    A4 d_a4;
    A5 d_a5;
};

template <typename A1, typename A2, typename A3, typename A4,
          typename A5, typename A6>
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                         A6, bslmf_Nil, bslmf_Nil, bslmf_Nil> {

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
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                                A6, A7, bslmf_Nil, bslmf_Nil> {

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
struct bslalg_BitwiseEqPassthroughTrait_PackedStruct_Imp<A1, A2, A3, A4, A5,
                                                       A6, A7, A8, bslmf_Nil> {

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

                // ===========================================
                // struct bslalg_BitwiseEqPassthroughTrait_Imp
                // ===========================================

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8, typename A9>
class bslalg_BitwiseEqPassthroughTrait_Imp {
    // Private implementation of 'bslalg_BitwiseEqPassthroughTrait' class.
    // This class computes whether the combination of parameterized types
    // 'A1 ... A9' result in a bitwise comparable type.

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;

    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    bslalg_HasTrait<A5, TRAIT>::VALUE &&
                    bslalg_HasTrait<A6, TRAIT>::VALUE &&
                    bslalg_HasTrait<A7, TRAIT>::VALUE &&
                    bslalg_HasTrait<A8, TRAIT>::VALUE &&
                    bslalg_HasTrait<A9, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) + sizeof(A8) +
                    sizeof(A9) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                   A1,A2,A3,A4,A5,A6,A7,A8,A9>)
    };

  public:
    typedef typename bslmf_If<HAS_TRAIT,
                              TRAIT,
                              bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT>
                             >::Type Type;
};

// Specializations
template <typename A1>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                               bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                               bslmf_Nil, bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;

  public:
    typedef typename bslmf_If<
                 bslalg_HasTrait<A1, TRAIT>::VALUE,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                           bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                                       A1, A2>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                                   A1, A2, A3>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                           bslmf_Nil, bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4 )==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                               A1, A2, A3, A4>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil,
                                           bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    bslalg_HasTrait<A5, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                           A1, A2, A3, A4, A5>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6,
                                           bslmf_Nil, bslmf_Nil, bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    bslalg_HasTrait<A5, TRAIT>::VALUE &&
                    bslalg_HasTrait<A6, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                       A1, A2, A3, A4, A5, A6>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6, A7,
                                           bslmf_Nil, bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    bslalg_HasTrait<A5, TRAIT>::VALUE &&
                    bslalg_HasTrait<A6, TRAIT>::VALUE &&
                    bslalg_HasTrait<A7, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                                   A1, A2, A3, A4, A5, A6, A7>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

template <typename A1, typename A2, typename A3, typename A4, typename A5,
          typename A6, typename A7, typename A8>
class bslalg_BitwiseEqPassthroughTrait_Imp<A1, A2, A3, A4, A5, A6, A7, A8,
                                           bslmf_Nil> {

    // PRIVATE TYPES
    typedef bslalg_TypeTraitBitwiseEqualityComparable TRAIT;
    enum {
        HAS_TRAIT = bslalg_HasTrait<A1, TRAIT>::VALUE &&
                    bslalg_HasTrait<A2, TRAIT>::VALUE &&
                    bslalg_HasTrait<A3, TRAIT>::VALUE &&
                    bslalg_HasTrait<A4, TRAIT>::VALUE &&
                    bslalg_HasTrait<A5, TRAIT>::VALUE &&
                    bslalg_HasTrait<A6, TRAIT>::VALUE &&
                    bslalg_HasTrait<A7, TRAIT>::VALUE &&
                    bslalg_HasTrait<A8, TRAIT>::VALUE &&
                    sizeof(A1) + sizeof(A2) + sizeof(A3) + sizeof(A4) +
                    sizeof(A5) + sizeof(A6) + sizeof(A7) + sizeof(A8) ==
                       sizeof(bslalg_BitwiseEqPassthroughTrait_PackedStruct<
                                               A1, A2, A3, A4, A5, A6, A7, A8>)
    };

  public:
    typedef typename bslmf_If<
                 HAS_TRAIT,
                 TRAIT,
                 bslalg_BitwiseEqPassthroughTrait_NotTrait<TRAIT> >::Type Type;
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
