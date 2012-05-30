// bslmf_isarray.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARRAY
#define INCLUDED_BSLMF_ISARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for array types.
//
//@CLASSES:
//  bslmf::IsArray: meta-function for detecting array types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's single type parameter is of array type.
// 'bslmf::IsArray' defines a 'VALUE' member that is initialized (at
// compile-time) to 1 if the parameter is an array type, or is a
// reference-to-array type, and to 0 otherwise.
//
///Usage
///-----
// For example:
//..
//  assert(1 == bslmf::IsArray<int    [5]>::VALUE);
//  assert(0 == bslmf::IsArray<int  *    >::VALUE);
//  assert(0 == bslmf::IsArray<int (*)[5]>::VALUE);
//..
// Note that the 'bslmf::IsArray' meta-function also evaluates to true (i.e.,
// 1) when applied to references to arrays:
//..
//  assert(1 == bslmf::IsArray<int (&)[5]>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // TBD Robo transitively needs this for 'bsl::atoi', etc.
#define INCLUDED_CSTDLIB
#endif

#endif

namespace BloombergLP {

namespace bslmf {

                         // ==============
                         // struct IsArray
                         // ==============

template <typename TYPE>
struct IsArray  : MetaInt<0> {
};

template <typename TYPE, std::size_t NUM_ELEMENTS>
struct IsArray<TYPE [NUM_ELEMENTS]> : MetaInt<1> {
};

template <typename TYPE>
struct IsArray<TYPE []> : MetaInt<1> {
};

template <typename TYPE>
struct IsArray<TYPE &> : IsArray<TYPE>::Type {
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsArray
#undef bslmf_IsArray
#endif
#define bslmf_IsArray bslmf::IsArray
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
