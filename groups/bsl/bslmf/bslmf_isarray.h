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
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's single type parameter is of array type.
// 'bslmf::IsArray' defines a 'value' member that is initialized (at
// compile-time) to 1 if the parameter is an array type, or is a
// reference-to-array type, and to 0 otherwise.
//
///Usage
///-----
// For example:
//..
//  assert(1 == bslmf::IsArray<int    [5]>::value);
//  assert(0 == bslmf::IsArray<int  *    >::value);
//  assert(0 == bslmf::IsArray<int (*)[5]>::value);
//..
// Note that the 'bslmf::IsArray' meta-function also evaluates to true (i.e.,
// 1) when applied to references to arrays:
//..
//  assert(1 == bslmf::IsArray<int (&)[5]>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
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

namespace bsl {

template <typename TYPE>
struct is_array : false_type
{};

template <typename TYPE, std::size_t NUM_ELEMENTS>
struct is_array<TYPE [NUM_ELEMENTS]> : true_type
{};

template <typename TYPE>
struct is_array<TYPE []> : true_type
{};

template <typename TYPE>
struct is_array<TYPE &> : is_array<TYPE>::type
{};

}

namespace BloombergLP {

namespace bslmf {

                         // ==============
                         // struct IsArray
                         // ==============

template <typename TYPE>
struct IsArray  : bsl::is_array<TYPE>::type
{};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsArray
#undef bslmf_IsArray
#endif
#define bslmf_IsArray bslmf::IsArray
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

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
