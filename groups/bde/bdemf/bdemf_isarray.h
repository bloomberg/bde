// bdemf_isarray.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_ISARRAY
#define INCLUDED_BDEMF_ISARRAY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for array types.
//
//@DEPRECATED: Use 'bslmf_isarray' instead.
//
//@CLASSES:
//    bdemf_IsArray: meta-function for detecting array types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's single type parameter is of array type.
// 'bdemf_IsArray' defines a 'VALUE' member that is initialized
// (at compile-time) to 1 if the parameter is an array, and to 0 otherwise.
//
///Usage
///-----
// For example:
//..
//   assert(1 == bdemf_IsArray<int    [5]>::VALUE);
//   assert(0 == bdemf_IsArray<int  *    >::VALUE);
//   assert(0 == bdemf_IsArray<int (*)[5]>::VALUE);
//..
// Note that the 'bdemf_IsArray' meta-function also evaluates to true (i.e., 1)
// when applied to references to arrays:
//..
//   assert(1 == bdemf_IsArray<int (&)[5]>::VALUE);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef bdemf_IsArray
#define bdemf_IsArray   bslmf_IsArray
#endif

namespace BloombergLP {

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
