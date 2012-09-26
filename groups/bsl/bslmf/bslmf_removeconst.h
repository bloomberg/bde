// bslmf_removeconst.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECONST
#define INCLUDED_BSLMF_REMOVECONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'const'-qualifier
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level 'const'-qualifier
//
//@SEE_ALSO: bslmf_addconst
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_const',
// that may be used to remove the top-level 'const'-qualifier from a type.
//
// 'bsl::remove_const' meets the requirements of the 'remove_const' template
// defined in the C++11 standard [meta.unary.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing The Const-Qualifier of A Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to strip the 'const'-qualifier from a particular type..
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we strip the the 'const'-qualifier from 'MyConstType' using
// 'bsl::remove_const' and verify that the resulting type is the same as
// 'MyType':
//..
//  assert(true ==
//        (bsl::is_same<bsl::remove_const<MyConstType>::type, MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

template <typename TYPE>
struct remove_const {
    // This 'struct' template implements the 'remove_const' meta-function
    // defined in the C++11 standard [meta.trans.cv].  This 'struct' template
    // provides a 'typedef' 'type' that has the same type as the (template
    // parameter) 'TYPE' except that any top-level 'const'-qualifier has been
    // removed.  Note that this generic default template provides a 'type' that
    // has the same type as 'TYPE' for when 'TYPE' is not 'const'-qualified.  A
    // template specialization is provided (below) that strips the
    // 'const'-qualifier for when 'TYPE' is 'const'-qualified.

    typedef TYPE type;
};

template <typename TYPE>
struct remove_const<TYPE const> {
     // This partial specialization of 'bsl::remove_const' provides a 'typedef'
     // 'type' that has the 'const'-qualifier removed.

    typedef TYPE type;
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
