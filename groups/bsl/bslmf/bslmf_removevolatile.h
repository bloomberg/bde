// bslmf_removevolatile.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#define INCLUDED_BSLMF_REMOVEVOLATILE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'volatile'-qualifier
//
//@CLASSES:
//  bsl::remove_volatile: meta-function for removing 'volatile'-qualifier
//
//@SEE_ALSO: bslmf_addvolatile
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_volatile',
// that may be used to remove the top-level 'volatile'-qualifier from a type.
//
// 'bsl::remove_volatile' meets the requirements of the 'remove_volatile'
// template defined in the C++11 standard [meta.unary.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing The Volatile-Qualifier of A Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to strip the 'volatile'-qualifier from a particular
// type..
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
//  typedef int          MyType;
//  typedef volatile int MyVolatileType;
//..
// Now, we strip the the 'volatile'-qualifier from 'MyVolatileType' using
// 'bsl::remove_volatile' and verify that the resulting type is the same as
// 'MyType':
//..
//  assert(true ==
//  (bsl::is_same<bsl::remove_volatile<MyVolatileType>::type, MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

template <typename TYPE>
struct remove_volatile {
    // This 'struct' template implements the 'remove_volatile' meta-function
    // defined in the C++11 standard [meta.trans.cv].  This 'struct' template
    // provides a 'typedef' 'type' that has the same type as the (template
    // parameter) 'TYPE' except that any top-level 'volatile'-qualifier has
    // been removed.  Note that this generic default template provides a 'type'
    // that has the same type as 'TYPE' for when 'TYPE' is not
    // 'volatile'-qualified.  A template specialization is provided (below)
    // that strips the 'volatile'-qualifier for when 'TYPE' is
    // 'volatile'-qualified.

    typedef TYPE type;
};

template <typename TYPE>
struct remove_volatile<TYPE volatile> {
     // This partial specialization of 'bsl::remove_volatile' provides a
     // 'typedef' 'type' that has the 'volatile'-qualifier removed.

    typedef TYPE type;
};

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
