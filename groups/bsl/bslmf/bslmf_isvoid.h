// bslmf_isvoid.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISVOID
#define INCLUDED_BSLMF_ISVOID

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for 'void' types.
//
//@CLASSES:
//  bslmf_IsVoid: meta-function for determining 'void' types
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a 'void' type.  'bslmf_IsVoid' defines a
// member, 'VALUE', whose value is initialized (at compile-time) to 1 if the
// parameter is a 'void' type (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

             // =======================================
             // private metafunction class bslmf_IsVoid
             // =======================================

template <class BDEMA_TYPE>
struct bslmf_IsVoid : bslmf_MetaInt<0> { };
    // This metafunction struct contains a nested 'VALUE' which converts to
    // 'true' if 'BDEMA_TYPE' is type 'void' and to 'false' otherwise.


template <>
struct bslmf_IsVoid<void>  : bslmf_MetaInt<1> { };
    // This specialization implements the metafunction when 'BDEMA_TYPE' is
    // 'void'.

template <>
struct bslmf_IsVoid<const void>  : bslmf_MetaInt<1> { };
    // This specialization implements the metafunction when 'BDEMA_TYPE' is
    // 'const void'.

template <>
struct bslmf_IsVoid<volatile void>  : bslmf_MetaInt<1> { };
    // This specialization implements the metafunction when 'BDEMA_TYPE' is
    // 'volatile void'.

template <>
struct bslmf_IsVoid<const volatile void>  : bslmf_MetaInt<1> { };
    // This specialization implements the metafunction when 'BDEMA_TYPE' is
    // 'const volatile void'.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
