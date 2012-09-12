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
//  bslalg::PassthroughTrait: pass-through trait mechanism
//  bslalg::PassthroughTraitBslmaAllocator: pass-through for 'bslma' allocators
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslalg::PassthroughTrait', for constructing a trait class that has a
// parameterized 'TRAIT' if a parameterized 'TYPE' has the 'TRAIT'.
//
// A similar meta-function, 'bslalg::PassthroughTraitBslmaAllocator', allows to
// compute the 'bslalg::TypeTraitUsesBslmaAllocator' trait for classes that
// have a parameterized 'ALLOCATOR' member.  This is most useful for STL
// containers.
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

namespace bslma { class Allocator; }

namespace bslalg {

template <typename TRAIT>
struct PassthroughTrait_NotTrait;

template <typename T, typename TRAIT>
struct PassthroughTrait_Imp;

                       // =======================
                       // struct PassthroughTrait
                       // =======================

template <typename T, typename TRAIT>
struct PassthroughTrait : PassthroughTrait_Imp<T, TRAIT>::Type {
    // If 'T' has 'TRAIT', then evaluate to 'TRAIT', else evaluate to a unique
    // class that is not 'TRAIT'.  Users of this meta-function do not need to
    // expand the result with '::Type' (though they may).

    typedef typename PassthroughTrait_Imp<T, TRAIT>::Type Type;
};

               // =====================================
               // struct PassthroughTraitBslmaAllocator
               // =====================================

template <typename ALLOCATOR>
struct PassthroughTraitBslmaAllocator :
           bslmf::If<bslmf::IsConvertible<bslma::Allocator*, ALLOCATOR>::VALUE,
                       TypeTraitUsesBslmaAllocator,
                       PassthroughTrait_NotTrait<TypeTraitUsesBslmaAllocator>
                    >::Type {
    // Trait that evaluates to 'TypeTraitUsesBslmaAllocator' if the
    // parameterized 'ALLOCATOR' is convertible from 'bslma::Allocator*'.
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

                  // ================================
                  // struct PassthroughTrait_NotTrait
                  // ================================

template <typename TRAIT>
struct PassthroughTrait_NotTrait {
    // Private class: Given a trait, this template produces a unique type which
    // is NOT the trait type and is not a trait at all.
};

                     // ===========================
                     // struct PassthroughTrait_Imp
                     // ===========================

template <typename T, typename TRAIT>
struct PassthroughTrait_Imp {
    // Private implementation of 'PassthroughTrait' class.
  private:
    enum { HAS_TRAIT = (int) HasTrait<T, TRAIT>::VALUE };

  public:
    typedef typename bslmf::If<HAS_TRAIT,
                                TRAIT,
                                PassthroughTrait_NotTrait<TRAIT>
                              >::Type Type;
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_PassthroughTraitBslmaAllocator
#undef bslalg_PassthroughTraitBslmaAllocator
#endif
#define bslalg_PassthroughTraitBslmaAllocator bslalg::PassthroughTraitBslmaAllocator
    // This alias is defined for backward compatibility.

#ifdef bslalg_PassthroughTrait
#undef bslalg_PassthroughTrait
#endif
#define bslalg_PassthroughTrait bslalg::PassthroughTrait
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
