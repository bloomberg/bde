// bdemf_forwardingtype.h          -*-C++-*-
#ifndef INCLUDED_BDEMF_FORWARDINGTYPE
#define INCLUDED_BDEMF_FORWARDINGTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining optimal forwarding type
//
//@DEPRECATED: Use 'bslmf_forwardingtype' instead.
//
//@CLASSES:
//   bdemf_ForwardingType: meta-function for determing optimal forwarding type
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bdemf_removecvq
//
//@DESCRIPTION: This component provides a meta function determining the most
// efficient forwarding type for a given template type 'TYPE'.
//
// For instance, on some platforms, non-modifiable references to user types
// should be taken by 'const&' to avoid a potentially expensive copy, but basic
// types (e.g., fundamental types, pointer types, function references and
// pointers) would be more efficiently taken by (non reference) value, even
// though this involves making a copy. This is because the copy can then be
// used internally by the function, whereas a 'const&' cannot and would have to
// be copied anyway (if used locally by the compiler), adding to the cost of
// passing the 'const&' argument.
//
// This component should not be used except when performance is of highest
// concern.
//
///Usage
///------
// The following example shows the result of instantiating
// 'bdemf_ForwardingType' on a number of different types.  First declare some
// types to compare:
//..
//  struct MyType {};
//  typedef int                    T1;
//  typedef int&                   T2;
//  typedef const volatile double& T3;
//  typedef const double &         T4;
//  typedef const float * &        T5;
//  typedef const float * const &  T6;
//  typedef MyType                 T7;
//  typedef const MyType&          T8;
//  typedef MyType&                T9;
//  typedef Mytype*                T10;
//..
// Next declare the expected result types:
//..
//  typedef int                    EXP1;
//  typedef int&                   EXP2;
//  typedef const volatile double& EXP3;
//  typedef double                 EXP4;
//  typedef const float * &        EXP5;
//  typedef const float *          EXP6;
//  typedef const MyType&          EXP7;
//  typedef const MyType&          EXP8;
//  typedef MyType&                EXP9;
//  typedef Mytype*                EXP10;
//..
// Finally, check that they match:
//..
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T1>::Type,  EXP1>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T2>::Type,  EXP2>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T3>::Type,  EXP3>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T4>::Type,  EXP4>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T5>::Type,  EXP5>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T6>::Type,  EXP6>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T7>::Type,  EXP7>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T8>::Type,  EXP8>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T9>::Type,  EXP9>::VALUE));
//  assert(1 == (bdemf_IsSame<bdemf_ForwardingType<T10>::Type, EXP10>::VALUE));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef bdemf_ForwardingType
#define bdemf_ForwardingType         bslmf_ForwardingType
    // This specialization is used when the template parameter of
    // 'bslmf_ForwardingType' is a 'const T&' and 'T' is not a volatile type.
    // In that case, it may be advantageous to pass an argument by value if 'T'
    // is a basic type, but not if it is a user-defined type.
#endif

#ifndef bdemf_ConstForwardingType
#define bdemf_ConstForwardingType    bslmf_ConstForwardingType
    // Use 'bslmf_ConstForwardingType<TYPE>' for forwarding an rvalue of the
    // given 'TYPE'.  Note that for lvalues and pointer types,
    // 'bslmf_ConstForwardingType<TYPE>' should produce the same type as the
    // 'bslmf_ForwardingType<TYPE>'.
#endif

namespace BloombergLP {

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
