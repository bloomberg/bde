// bslmf_AddReference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDREFERENCE
#define INCLUDED_BSLMF_ADDREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding reference-ness to a type.
//
//@CLASSES:
//    bslmf_AddReference: meta-function to form a reference to a type
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// form a reference to a type from its single template type parameter.  Types
// that are already reference types are unmodified, as are 'void' types.
//
///Usage
///-----
// For example:
//..
//   struct MyType {};
//   typedef MyType& MyTypeRef;
//
//   bslmf_AddReference<int             >::Type x1; // int&
//   bslmf_AddReference<int&            >::Type x2; // int&
//   bslmf_AddReference<int volatile    >::Type x3; // volatile int&
//   bslmf_AddReference<int volatile&   >::Type x4; // volatile int&
//
//   bslmf_AddReference<MyType          >::Type     // MyType&
//   bslmf_AddReference<MyType&         >::Type     // MyType&
//   bslmf_AddReference<MyTypeRef       >::Type     // MyType&
//   bslmf_AddReference<MyType const    >::Type     // const MyType&
//   bslmf_AddReference<MyType const&   >::Type     // const MyType&
//   bslmf_AddReference<const MyTypeRef >::TYPE     // MyType&
//   bslmf_AddReference<const MyTypeRef&>::TYPE     // MyType& (REQUIRES C++11)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

                        // =========================
                        // struct bslmf_AddReference
                        // =========================


template <class BSLMF_TYPE>
struct bslmf_AddReference {
    // This class defines some basic traits used by 'bdema_ManagedPtr'.
    // It is primarily used to allow managed pointers of type 'void'
    // to work properly.
    typedef BSLMF_TYPE& Type;
};

template <class BSLMF_TYPE>
struct bslmf_AddReference<BSLMF_TYPE &> {
    typedef BSLMF_TYPE& Type;
};

template <>
struct bslmf_AddReference<void> {
    // This specialization of 'bslmf_AddReference' for type
    // 'void' allows to avoid declaring a reference to 'void'.

    typedef void Type;
};

template <>
struct bslmf_AddReference<const void> {
    // This specialization of 'bslmf_AddReference' for type
    // 'const void' allows to avoid declaring a reference to 'const void'.

    typedef const void Type;
};

template <>
struct bslmf_AddReference<volatile void> {
    // This specialization of 'bslmf_AddReference' for type
    // 'volatile void' allows to avoid declaring a reference to 'volatile void'.

    typedef volatile void Type;
};

template <>
struct bslmf_AddReference<const volatile void> {
    // This specialization of 'bslmf_AddReference' for type
    // 'const volatile void' allows to avoid declaring a reference to
    // 'const volatile void'.

    typedef const volatile void Type;
};

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
