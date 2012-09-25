// bslmf_removecv.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECV
#define INCLUDED_BSLMF_REMOVECV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level cv-qualifier
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level cv-qualifier
//
//@SEE_ALSO: bslmf_addcv
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_cv', that
// may be used to remove the top-level cv-qualifier ('const'-qualifier and
// 'volatile'-qualifier) from a type.
//
// 'bsl::remove_const' meets the requirements of the 'remove_cv' template
// defined in the C++11 standard [meta.unary.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing The CV-Qualifier of A Type
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to strip the cv-qualifier from a particular type..
//
// First, we create two 'typedef's -- a cv-qualified type ('MyCvType') and the
// same type without the cv-qualifier ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we strip the the cv-qualifier from 'MyCvType' using 'bsl::remove_cv'
// and verify that the resulting type is the same as 'MyType':
//..
//  assert(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
//                                                            MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECONST
#include <bslmf_removeconst.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#include <bslmf_removevolatile.h>
#endif

namespace bsl {

template <typename TYPE>
struct remove_cv {
    // This 'struct' template implements the 'remove_cv' meta-function defined
    // in the C++11 standard [meta.trans.cv] to provide a 'typedef' 'type' that
    // has the same type as the (template parameter) 'TYPE' except that any
    // top-level cv-qualifier has been removed.

    typedef typename remove_const<typename remove_volatile<TYPE>::type>::type
                                                                          type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE' except that any top-level cv-qualifier has been
        // removed.
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
