// bdemf_arraytopointer.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_ARRAYTOPOINTER
#define INCLUDED_BDEMF_ARRAYTOPOINTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta function to convert array type to pointer types.
//
//@DEPRECATED: Use 'bslmf_arraytopointer' instead.
//
//@CLASSES:
//         bdemf_ArrayToPointer: Convert an array type to a pointer type
//    bdemf_ArrayToConstPointer: Conver an array type to a const pointer type
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO:
//    bdemf_IsArray
//    bdemf_ForwardingType
//
//@DESCRIPTION: This component provides a meta function for converting array
// types to pointer types. The utility is generally used for in templates that
// require forwarding or storarge of arguments that are passed a arrays(e.g.
// "string literals").  For non array types, the type is left unmodified.
// Note that 'bdemf_ArrayToPointer' and 'bdemf_ArrayToConstPointer' retain
// the CV qualifiers of the original type.  In other words, if the original
// array type was const or volatile, or const volatile, the converted
// pointer type will also be const, volatile, or const volatile respectively.
// When an explict const pointer pointer type is needed(such as when accepting
// as argument, then 'bdemf_ArrayToConstPointer' should be used.
//
///Usage
///-----
// For example:
//..
//   assert(1 == bdemf_IsSame<bdemf_ArrayToPointer<int[5]>::Type, int*>::VALU);
//   assert(1 == bdemf_IsSame<bdemf_ArrayToPointer<int*>::Type, int*>::VALUE);
//   assert(0 == bdemf_IsSame<bdemf_ArrayToPointer<int(*)[5]>::Type,
//                                                 int**>::VALUE);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ARRAYTOPOINTER
#include <bslmf_arraytopointer.h>
#endif

#ifndef bdemf_ArrayToPointer
#define bdemf_ArrayToPointer      bslmf_ArrayToPointer
#endif

#ifndef bdemf_ArrayToConstPointer
#define bdemf_ArrayToConstPointer bslmf_ArrayToConstPointer
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
