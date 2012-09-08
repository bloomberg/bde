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
//  bslmf::IsVoid: meta-function for determining 'void' types
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component defines a simple template 'struct' used to
// evaluate whether its parameter is a 'void' type.  'bslmf::IsVoid' defines a
// member, 'VALUE', whose value is initialized (at compile-time) to 1 if the
// parameter is a 'void' type (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Example 1: using ::VALUE...
// - - - - - - - - - - - - - -
//
//
// Example 2: using ::Type...
// - - - - - - - - - - - - - -
//
//
// Example 3: Tag dispatch...
// - - - - - - - - - - - - - -
//

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_void : false_type
{
};

template <>
struct is_void<void> : true_type
{
};

template <>
struct is_void<const void> : true_type
{
};

template <>
struct is_void<volatile void> : true_type
{
};

template <>
struct is_void<const volatile void> : true_type
{
};

}  // close namespace bsl

namespace BloombergLP {

namespace bslmf
{

             // ===================
             // struct bslmf_IsVoid
             // ===================

template<class TYPE>
struct IsVoid : MetaInt<bsl::is_void<TYPE>::value>
    // This metafunction struct contains a nested 'VALUE' which converts to
    // 'true' if 'BSLMF_TYPE' is type 'void' and to 'false' otherwise, and
    // a nested type alias 'TYPE' corresponding to 'bslmf::MetaInt<VALUE>'.
{
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
