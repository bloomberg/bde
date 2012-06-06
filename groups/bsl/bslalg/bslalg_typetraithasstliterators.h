// bslalg_typetraithasstliterators.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with STL-like iterators.
//
//@CLASSES:
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasStlIterators'.  A 'TYPE' that has this trait fulfills
// the following requirements, where 'mX' is a modifiable object and 'X' a
// non-modifiable object of 'TYPE':
//..
//  Valid expression     Type              Note
//  ----------------     ----              ----
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).
//
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).  The value type of this
//                                         iterator is not modifiable.
//
//  mX.begin()           iterator          Similar to standard containers
//  mX.end()             iterator
//  X.begin()            const_iterator
//  X.end()              const_iterator
//  X.cbegin()           const_iterator
//  X.cend()             const_iterator
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        //================================
                        // struct TypeTraitHasStlIterators
                        //================================

struct TypeTraitHasStlIterators {
    // A type with this trait defines (at minimum) the nested types 'iterator'
    // and 'const_iterator' and the functions 'begin()' and 'end()' having the
    // standard STL semantics.
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::TypeTraitHasStlIterators bslalg_TypeTraitHasStlIterators;
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
