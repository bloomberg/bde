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
//    bslmf_IsArray: meta-function for detecting array types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's single type parameter is of array type.
// 'bslmf_IsArray' defines a 'VALUE' member that is initialized
// (at compile-time) to 1 if the parameter is an array, and to 0 otherwise.
//
///Usage
///-----
// For example:
//..
//   assert(1 == bslmf_IsArray<int    [5]>::VALUE);
//   assert(0 == bslmf_IsArray<int  *    >::VALUE);
//   assert(0 == bslmf_IsArray<int (*)[5]>::VALUE);
//..
// Note that the 'bslmf_IsArray' meta-function also evaluates to true (i.e., 1)
// when applied to references to arrays:
//..
//   assert(1 == bslmf_IsArray<int (&)[5]>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
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

                         // ========================
                         // struct bslmf_IsArray_Imp
                         // ========================

template <typename TYPE>
struct bslmf_IsArray_Imp {
    enum { VALUE = 0 };
};

template <typename TYPE, std::size_t NUM_ELEMENTS>
struct bslmf_IsArray_Imp<TYPE [NUM_ELEMENTS]> {
    enum { VALUE = 1 };
};

template <typename TYPE>
struct bslmf_IsArray_Imp<TYPE []> {
    enum { VALUE = 1 };
};

                         // ====================
                         // struct bslmf_IsArray
                         // ====================

template <typename TYPE>
struct bslmf_IsArray {
    enum { VALUE = bslmf_IsArray_Imp<TYPE>::VALUE };
};

template <typename TYPE>
struct bslmf_IsArray<TYPE &> {
    enum { VALUE = bslmf_IsArray_Imp<TYPE>::VALUE };
};

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
