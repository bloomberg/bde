// bslmf_addpointer.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDPOINTER
#define INCLUDED_BSLMF_ADDPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides meta-function to tranform a type to a pointer to that type
//
//@CLASSES:
//  bsl::add_pointer: meta-function to transform  to a pointer type
//
//@SEE_ALSO: bslmf_removepointer
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_pointer',
// that may be used to transform a type to a pointer to that type.
//
// 'bsl::add_pointer' meets the requirements of the 'add_pointer' template
// defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform Type to Pointer Type to that Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a type to a pointer type to that type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType')
// and the type pointed to by the pointer type ('MyType'):
//..
//  typedef int   MyType;
//  typedef int * MyPtrType;
//..
// Now, we transform 'MyType' to a pointer type to 'MyType' using
// 'bsl::add_pointer' and verify that the resulting type is the same as
// 'MyPtrType':
//..
//  assert((bsl::is_same<bsl::add_pointer<MyType>::type,
//                       MyPtrType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

namespace bsl {

                         // ==================
                         // struct add_pointer
                         // ==================

template <typename TYPE>
struct add_pointer {
    // This 'struct' template implements the 'add_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr], providing an alias
    // ,'type', that returns the result.  If the (template parameter) 'TYPE' is
    // not a reference type, then 'type' is an alias to a pointer type that
    // points to 'TYPE'; otherwise, 'type' is an alias to a pointer type that
    // points to the type referred to by the reference 'TYPE'.

    typedef typename remove_reference<TYPE>::type *type;
        // This 'typedef' is an alias to a pointer type that points to the
        // (template parameter) 'TYPE' if it's not a reference type; otherwise,
        // this 'typedef' is an alias to a pointer type that points to the type
        // referred to by the reference 'TYPE'.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
