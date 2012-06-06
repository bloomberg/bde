// bslalg_hastrait.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASTRAIT
#define INCLUDED_BSLALG_HASTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for checking compile-time trait.
//
//@CLASSES:
//  bslalg::HasTrait: trait detection mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a meta-function, 'bslalg::HasTrait',
// for for macros used to assign traits to user-defined class.  Traits are used
// to enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bslalg::TypeTraitBitwiseMoveable' may
// allow resizing an array of objects by simply calling 'std::memcpy' instead
// of invoking a copy-constructor on every object.  The usage example shows how
// to use the 'bslalg::TypeTraitUsesBslmaAllocator' to propagate allocators to
// nested instances that may require them.
//
// This component should be used in conjunction with other components from the
// package 'bslalg'.  See the package-level documentation for an overview.  The
// most useful classes and macros defined in this component are:
//..
//  bslalg::HasTrait<TYPE, TRAIT>             This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {

namespace bslalg {

                       // ===============
                       // struct HasTrait
                       // ===============

template <typename TYPE, typename TRAIT>
struct HasTrait {
    // This meta-function evaluates to 'bslmf::MetaInt<1>' if the parameterized
    // type 'TYPE' has the parameterized 'TRAIT', and to 'bslmf::MetaInt<0>'
    // otherwise.  Note that 'TYPE' "has" the 'TRAIT' if
    // 'bslalg_TypeTraits<TYPE>' directly includes 'TRAIT' or else includes a
    // trait that implies 'TRAIT'.

  private:
    typedef typename bslmf::RemoveCvq<TYPE>::Type  NoCvqType;
    typedef bslalg_TypeTraits<NoCvqType>           NoCvqTraits;

  public:
    enum {
        VALUE = bslmf::IsConvertible<NoCvqTraits, TRAIT>::VALUE
    };

    typedef bslmf::MetaInt<VALUE> Type;
};

template <typename TYPE>
struct HasTrait<TYPE, TypeTraitBitwiseMoveable> {
    // bitwise copyable                => bitwise moveable
    // has trivial default constructor => bitwise moveable

  private:
    typedef typename bslmf::RemoveCvq<TYPE>::Type  NoCvqType;
    typedef bslalg_TypeTraits<NoCvqType>           NoCvqTraits;

  public:
    enum {
        VALUE = bslmf::IsConvertible<NoCvqTraits,
                                    TypeTraitBitwiseMoveable>::VALUE
             || bslmf::IsConvertible<NoCvqTraits,
                                    TypeTraitBitwiseCopyable>::VALUE
             || bslmf::IsConvertible<NoCvqTraits,
                           TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    typedef bslmf::MetaInt<VALUE> Type;
};

template <typename TYPE>
struct HasTrait<TYPE, TypeTraitBitwiseCopyable> {
    // has trivial default constructor => bitwise copyable

  private:
    typedef typename bslmf::RemoveCvq<TYPE>::Type  NoCvqType;
    typedef bslalg_TypeTraits<NoCvqType>           NoCvqTraits;

  public:
    enum {
        VALUE = bslmf::IsConvertible<NoCvqTraits,
                                    TypeTraitBitwiseCopyable>::VALUE
             || bslmf::IsConvertible<NoCvqTraits,
                           TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    typedef bslmf::MetaInt<VALUE> Type;
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_HasTrait
#undef bslalg_HasTrait
#endif
#define bslalg_HasTrait bslalg::HasTrait
    // This alias is defined for backward compatibility.

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
