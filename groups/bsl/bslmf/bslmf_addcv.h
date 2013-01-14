// bslmf_addcv.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCV
#define INCLUDED_BSLMF_ADDCV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding top-level cv-qualifiers.
//
//@CLASSES:
//  bsl::add_const: meta-function for adding top-level cv-qualifiers
//
//@SEE_ALSO: bslmf_removeconst
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_cv', that may
// be used to add a top-level 'const'-qualifier and a 'volatile'-qualifier to a
// type if it is not a reference type, nor a function type, nor already
// 'const'-qualified and 'volatile'-qualified at the top-level.
//
// 'bsl::add_cv' meets the requirements of the 'add_cv' template defined in the
// C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'const'-qualifier and 'volatile'-qualifier to a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier and a 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we add a 'const'-qualifier and a 'volatile'-qualifier to 'MyType' using
// 'bsl::add_cv' and verify that the resulting type is the same as 'MyCvType':
//..
//  assert(true == (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDCONST
#include <bslmf_addconst.h>
#endif

#ifndef INCLUDED_BSLMF_ADDVOLATILE
#include <bslmf_addvolatile.h>
#endif

namespace bsl {

                         // =============
                         // struct add_cv
                         // =============

template <typename TYPE>
struct add_cv {
    // This 'struct' template implements the 'add_cv' meta-function defined in
    // the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  If the the (template parameter) 'TYPE' is not a
    // reference type, nor a function type, nor already 'const'-qualified and
    // 'volatile'-qualified at the top-level, then 'type' is an alias to 'TYPE'
    // with the top-level 'const'-qualifier and 'volatile'-qualifier added;
    // otherwise, 'type' is an alias to 'TYPE'.

    // PUBLIC TYPES
    typedef typename add_const<typename add_volatile<TYPE>::type>::type type;
        // This 'typedef' is an alias alias to the (template parameter) 'TYPE'
        // with a top-level 'const'-qualifier and 'volatile'-qualifier added if
        // 'TYPE' is not a reference type, nor a function type, nor already
        // 'const'-qualified and 'volatile'-qualified at the top-level;
        // otherwise, 'type' is an alias to 'TYPE'.

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
