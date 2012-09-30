// bslmf_addcv.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCV
#define INCLUDED_BSLMF_ADDCV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level cv-qualifier
//
//@CLASSES:
//  bsl::add_const: meta-function for adding a top-level cv-qualifier
//
//@SEE_ALSO: bslmf_removeconst
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_cv', that may
// be used to add a top-level 'const'-qualifier and a 'volatile'-qualifier to a
// type if it is not a reference, a function or already 'const'-qualified and
// 'volatile'-qualified at the top-level.
//
// 'bsl::add_cv' meets the requirements of the 'add_cv' template defined in the
// C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding The 'const'-qualifier and 'volatile'-qualifier to A Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add the 'const'-qualifier and 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we add the the 'const'-qualifier and the 'volatile'-qualifier to
// 'MyType' using 'bsl::add_cv' and verify that the resulting type is the
// same as 'MyCvType':
//..
//  assert(true ==
//                 (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
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
    // the C++11 standard [meta.trans.cv] to provide a 'typedef' 'type'.  If
    // the (template parameter) 'TYPE' is not a reference, nor a function, nor
    // already 'const'-qualified and 'volatile'-qualified, then 'type' is an
    // alias to the same type as 'TYPE' except that the top-level
    // 'const'-qualifier and 'volatile'-qualifier has been added; otherwise,
    // 'type' is an alias to 'TYPE'.

    typedef typename add_const<typename add_volatile<TYPE>::type>::type type;
        // This 'typedef' is an alias alias to the same type as the (template
        // parameter) 'TYPE' except that the top-level 'const'-qualifier and
        // 'volatile'-qualifier has been added if 'TYPE' is not a reference,
        // nor a function, nor already 'const'-qualified and
        // 'volatile'-qualified; otherwise, 'type' is an alias to 'TYPE'.

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
