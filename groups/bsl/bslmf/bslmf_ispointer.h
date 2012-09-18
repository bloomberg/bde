// bslmf_ispointer.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTER
#define INCLUDED_BSLMF_ISPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@CLASSES:
//  bsl::is_pointer: standard meta-function for determining pointer types
//  bsl::IsPointer: meta-function for determining pointer types
//
//@SEE_ALSO: bslmf_integerconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_pointer'
// and 'BloombergLP::bslmf::IsPointer', both of which may be used to query
// whether a type is a pointer type.
//
// 'bsl::is_pointer' meets the requirements of the 'is_pointer' template
// defined in the C++11 standard [meta.unary.cat], while 'bslmf::IsPointer' was
// devised before 'is_pointer' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_pointer' is indicated by the
// class member 'value', while the result for 'bslmf::IsPointer' is indicated
// by the class member 'VALUE'.
//
// Note that 'bsl::is_pointer' should be preferred over 'bslmf::IsPointer', and
// in general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a pointer type.
//
// First, we create two 'typedef's -- a pointer type and a non-pointer type:
//..
//  typedef int  MyType;
//  typedef int *MyPtrType;
//..
// Now, we instantiate the 'bsl::is_pointer' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_pointer<MyType>::value);
//  assert(true == bsl::is_pointer<MyPtrType>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {
                         // ====================
                         // struct IsPointer_Imp
                         // ====================

template <class TYPE>
struct IsPointer_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a (non-cv-qualified) pointer type.  This
    // generic default template derives from 'bsl::false_type'.  A template
    // specialization is provided (below) that derives from 'bsl::true_type'.
};

                         // ============================
                         // struct IsPointer_Imp<TYPE *>
                         // ============================

template <class TYPE>
struct IsPointer_Imp<TYPE *> : bsl::true_type {
     // This partial specialization of 'IsPointer_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a pointer
     // type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                         // =================
                         // struct is_pointer
                         // =================

template <class TYPE>
struct is_pointer
    : BloombergLP::bslmf::IsPointer_Imp<typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_pointer' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is a pointer.  This 'struct' derives from
    // 'bsl::true_type' if the 'TYPE' is a pointer type (but not a pointer to
    // non-static member), and 'bsl::false_type' otherwise.

};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <class TYPE>
struct IsPointer : MetaInt<bsl::is_pointer<TYPE>::value> {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'TYPE' is a pointer type.  This 'struct' derives
    // from 'bslmf::MetaInt<1>' if the 'TYPE' is a pointer type (but not a
    // pointer to non-static member), and 'bslmf::MetaInt<0>' otherwise.
    //
    // Note that although this 'struct' is functionally identical to
    // 'bsl::is_pointer', and the use of 'bsl::is_pointer' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

// ===========================================================================
//                           BACKWARD-COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPointer
#undef bslmf_IsPointer
#endif
#define bslmf_IsPointer bslmf::IsPointer
    // This alias is defined for backward-compatibility.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
