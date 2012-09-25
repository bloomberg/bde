// bslmf_addconst.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCONST
#define INCLUDED_BSLMF_ADDCONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level 'const'-qualifier
//
//@CLASSES:
//  bsl::add_const: meta-function for adding a top-level 'const'-qualifier
//
//@SEE_ALSO: bslmf_removeconst
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_const', that
// may be used to add a top-level 'const'-qualifier to a type if it is not a
// reference, function or already 'const'-qualified at the top-level.
//
// 'bsl::add_const' meets the requirements of the 'add_const' template defined
// in the C++11 standard [meta.unary.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding The Const-Qualifier to A Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add the 'const'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we add the the 'const'-qualifier to 'MyType' using 'bsl::add_const' and
// verify that the resulting type is the same as 'MyConstType':
//..
//  assert(true ==
//           (bsl::is_same<bsl::add_const<MyType>::type, MyConstType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONST
#include <bslmf_isconst.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE, bool ADD_CONST>
struct AddConst_Imp {
    // This 'struct' template provides an alias 'Type' that add the
    // 'const'-qualifier to the (template parameter) 'TYPE' if the (template
    // parameter) 'ADD_CONST' is 'true'.  This generic default template adds
    // the 'const'-qualifier to 'TYPE'.  A template specialization (below) does
    // not modify 'TYPE'.

    typedef TYPE const Type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'const'-qualifier added.
};

template <typename TYPE>
struct AddConst_Imp<TYPE, false> {
    // This partial specialization of 'AddConst_Imp' provides an alias 'Type'
    // that has the same type as 'TYPE' for when the (template parameter)
    // 'ADD_CONST' is 'false'.

    typedef TYPE Type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'const'-qualifier added.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct add_const {
    // This 'struct' template implements the 'add_const' meta-function defined
    // in the C++11 standard [meta.unary.cv] to provide an alias 'type' that
    // has the same type as the (template parameter) 'TYPE' except that the
    // top-level 'const'-qualifier has been added, unless 'TYPE' is a
    // reference, a function, or already 'const'-qualified at the top-level.

    typedef typename BloombergLP::bslmf::AddConst_Imp<
                            TYPE,
                            !is_reference<TYPE>::value
                            && !is_function<TYPE>::value
                            && !is_const<TYPE>::value>::Type type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'const'-qualifier added.
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
