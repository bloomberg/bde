// bslmf_addvolatile.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDVOLATILE
#define INCLUDED_BSLMF_ADDVOLATILE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level 'volatile'-qualifier
//
//@CLASSES:
//  bsl::add_volatile: meta-function for adding top-level 'volatile'-qualifier
//
//@SEE_ALSO: bslmf_removevolatile
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_volatile',
// that may be used to add a top-level 'volatile'-qualifier to a type if it is
// not a reference, function or already 'volatile'-qualified at the top-level.
//
// 'bsl::add_volatile' meets the requirements of the 'add_volatile' template
// defined in the C++11 standard [meta.unary.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding The Volatile-Qualifier to A Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add the 'volatile'-qualifier to a particular type..
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
//  typedef int          MyType;
//  typedef volatile int MyVolatileType;
//..
// Now, we add the the 'volatile'-qualifier to 'MyType' using
// 'bsl::add_volatile' and verify that the resulting type is the same as
// 'MyVolatileType':
//..
//  assert(true ==
//     (bsl::is_same<bsl::add_volatile<MyType>::type, MyVolatileType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISVOLATILE
#include <bslmf_isvolatile.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE, bool ADD_VOLATILE>
struct AddVolatile_Imp {
    // This 'struct' template provides an alias 'Type' that add the
    // 'volatile'-qualifier to the (template parameter) 'TYPE' if the (template
    // parameter) 'ADD_VOLATILE' is 'true'.  This generic default template adds
    // the 'volatile'-qualifier to 'TYPE'.  A template specialization (below)
    // does not modify 'TYPE'.

    typedef TYPE volatile Type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'volatile'-qualifier added.
};

template <typename TYPE>
struct AddVolatile_Imp<TYPE, false> {
    // This partial specialization of 'AddVolatile_Imp' provides an alias
    // 'Type' that has the same type as 'TYPE' for when the (template
    // parameter) 'ADD_VOLATILE' is 'false'.

    typedef TYPE Type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'volatile'-qualifier added.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct add_volatile {
    // This 'struct' template implements the 'add_volatile' meta-function
    // defined in the C++11 standard [meta.unary.cv] to provide an alias 'type'
    // that has the same type as the (template parameter) 'TYPE' except that
    // the top-level 'volatile'-qualifier has been added, unless 'TYPE' is a
    // reference, a function, or already 'volatile'-qualified at the top-level.

    typedef typename BloombergLP::bslmf::AddVolatile_Imp<
                            TYPE,
                            !is_reference<TYPE>::value
                            && !is_function<TYPE>::value
                            && !is_volatile<TYPE>::value>::Type type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with 'volatile'-qualifier added.
};

}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
