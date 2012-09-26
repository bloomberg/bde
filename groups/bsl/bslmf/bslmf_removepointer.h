// bslmf_removepointer.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#define INCLUDED_BSLMF_REMOVEPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Transform a pointer type to type pointed by pointer type
//
//@CLASSES:
//  bsl::remove_const: transform a pointer type to type pointed by pointer type
//
//@SEE_ALSO: bslmf_addpointer
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_pointer',
// that may be used to transform a pointer type to the type pointed to by the
// pointer type.
//
// 'bsl::remove_pointer' meets the requirements of the 'remove_pointer'
// template defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform Pointer Type to The Type Pointed to By the Pointer Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a pointer type to the type pointed to by
// that poiner type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType')
// and the type pointed to by the pointer type ('MyType'):
//..
//  typedef int   MyType;
//  typedef int * MyPtrType;
//..
// Now, we get the type pointed to by 'MyPtrType' using 'bsl::remove_pointer'
// and verify that the resulting type is the same as 'MyType':
//..
//  assert((bsl::is_same<bsl::remove_pointer<MyPtrType>::type,
//                       MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {
namespace bslmf {

                         // ========================
                         // struct RemovePointer_Imp
                         // ========================

template <typename TYPE>
struct RemovePointer_Imp {
    // This 'struct' template provides a 'typedef' 'Type' that is an alias to
    // the type pointed to by the (template parameter) 'TYPE' if 'TYPE' is a
    // (non-cv-qualified) pointer type.  Otherwise, 'Type' is an alias to
    // 'TYPE'.  This generic default's 'Type' is always an alias to 'TYPE'.  A
    // template specialization (below) that handles the case of when 'TYPE' is
    // a pointer type.

    typedef TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

template <typename TYPE>
struct RemovePointer_Imp<TYPE *> {
     // This partial specialization of 'RemovePointer_Imp' for when the
     // (template parameter) is a pointer 'TYPE' provides a 'typedef' 'TYPE'
     // that is an alias to the type pointed to by 'TYPE'.

    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct remove_pointer {
    // This 'struct' template implements the 'remove_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr] to provide a 'typedef'
    // 'type'.  'type' is an alias to the type pointed to by the (template
    // parameter) 'TYPE' if 'TYPE' is a (possibly cv-qualified) pointer type.
    // Otherwise.  Otherwise, 'type' is an alias to 'TYPE'.

    typedef typename BloombergLP::bslmf::RemovePointer_Imp<
                typename remove_cv<TYPE>::type>::Type
                                                                          type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE' if 'TYPE' is a (possibly cv-qualified) pointer
        // type.  Otherwise.  Otherwise, 'type' is an alias to 'TYPE'.
};

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
