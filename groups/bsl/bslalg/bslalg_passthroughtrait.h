// bslalg_passthroughtrait.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#define INCLUDED_BSLALG_PASSTHROUGHTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for computing compile-time traits.
//
//@CLASSES:
//                bslalg_PassthroughTrait: pass-through trait mechanism
//  bslalg_PassthroughTraitBslmaAllocator: pass-through for 'bslma' allocators
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslalg_PassthroughTrait', for constructing a trait class that has a
// parameterized 'TRAIT' if a parameterized 'TYPE' has the 'TRAIT'.
//
// A similar meta-function, 'bslalg_PassthroughTraitBslmaAllocator', allows to
// compute the 'bslalg_TypeTraitUsesBslmaAllocator' trait for classes that have
// a parameterized 'ALLOCATOR' member.  This is most useful for STL containers.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#include <bslalg_typetraitnil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

template <typename TRAIT>
struct bslalg_PassthroughTrait_NotTrait;

template <typename T, typename TRAIT>
struct bslalg_PassthroughTrait_Imp;

                       // ==============================
                       // struct bslalg_PassthroughTrait
                       // ==============================

template <typename T, typename TRAIT>
struct bslalg_PassthroughTrait : bslalg_PassthroughTrait_Imp<T, TRAIT>::Type {
    // If 'T' has 'TRAIT', then evaluate to 'TRAIT', else evaluate to a unique
    // class that is not 'TRAIT'.  Users of this meta-function do not need to
    // expand the result with '::Type' (though they may).

    typedef typename bslalg_PassthroughTrait_Imp<T, TRAIT>::Type Type;
};

               // ============================================
               // struct bslalg_PassthroughTraitBslmaAllocator
               // ============================================

template <typename ALLOCATOR>
struct bslalg_PassthroughTraitBslmaAllocator :
              bslmf_If<bslmf_IsConvertible<bslma_Allocator*, ALLOCATOR>::VALUE,
                       bslalg_TypeTraitUsesBslmaAllocator,
                       bslalg_PassthroughTrait_NotTrait<
                                            bslalg_TypeTraitUsesBslmaAllocator>
                      >::Type {
    // Trait that evaluates to 'bslalg_TypeTraitUsesBslmaAllocator' if
    // the parameterized 'ALLOCATOR' is convertible from 'bslma_Allocator*'.
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

                  // =======================================
                  // struct bslalg_PassthroughTrait_NotTrait
                  // =======================================

template <typename TRAIT>
struct bslalg_PassthroughTrait_NotTrait {
    // Private class: Given a trait, this template produces a unique type
    // which is NOT the trait type and is not a trait at all.
};

                     // ==================================
                     // struct bslalg_PassthroughTrait_Imp
                     // ==================================

template <typename T, typename TRAIT>
struct bslalg_PassthroughTrait_Imp {
    // Private implementation of 'bslalg_PassthroughTrait' class.
  private:
    enum { HAS_TRAIT = (int) bslalg_HasTrait<T, TRAIT>::VALUE };

  public:
    typedef typename bslmf_If<HAS_TRAIT,
                              TRAIT,
                              bslalg_PassthroughTrait_NotTrait<TRAIT>
                             >::Type Type;
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
