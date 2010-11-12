// bdealg_typetraithasstliterators.h   -*-C++-*-
#ifndef INCLUDED_BDEALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BDEALG_TYPETRAITHASSTLITERATORS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with STL-like iterators.
//
//@DEPRECATED: Use 'bslalg_typetraithasstliterators' instead.
//
//@CLASSES:
//  bdealg_TypeTraitHasStlIterators: has STL-like iterators
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION:  This component provides a single traits class,
// 'bdealg_TypeTraitHasStlIterators'.  A 'TYPE' that has this trait fulfills
// the following requirements, where 'mX' is a modifiable instance and 'X' a
// non-modifiable instance of 'TYPE':
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

namespace BloombergLP {

                        //=======================================
                        // struct bdealg_TypeTraitHasStlIterators
                        //=======================================

typedef bslalg_TypeTraitHasStlIterators bdealg_TypeTraitHasStlIterators;
    // A type with this trait defines (at minimum) the nested types 'iterator'
    // and 'const_iterator' and the functions 'begin()' and 'end()' having the
    // standard STL semantics.

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
