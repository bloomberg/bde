// bslmf_addrvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#define INCLUDED_BSLMF_ADDRVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to rvalue reference.
//
//@CLASSES:
//  bsl::add_rvalue_reference: standard meta-function for transforming types
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_addlvaluereference
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_rvalue_reference', that may be used to transform a type to its
// rvalue reference type.
//
// 'bsl::add_rvalue_reference' meets the requirements of the
// 'add_rvalue_reference' template defined in the C++11 standard
// [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform to Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform some types to rvalue reference types.
//
// Now, for a set of types, we transform each type to the corresponding rvalue
// reference of that type using 'bsl::add_rvalue_reference' and verify the
// result:
//..
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int&&>::value));
//  assert(false ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int  >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&>::type,  int& >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&&>::type, int&&>::value));
//#endif
//..
// Note that rvalue reference was introduced in C++11 and may not be supported
// by all compilers.  Note also that according to 'reference collapsing'
// semantics [8.3.2], 'add_rvalue_reference' does not transform 'TYPE' to
// rvalue reference type if 'TYPE' is an lvalue reference type.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace bsl {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class TYPE>
struct add_rvalue_reference {
    // This 'struct' template implements a meta-function to transform the
    // (template parameter) 'TYPE' to its rvalue reference type.

    // PUBLIC TYPES
    typedef TYPE&& type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void' type.

    // PUBLIC TYPES
    typedef void type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void const> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const' type.

    // PUBLIC TYPES
    typedef void const type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void volatile' type.

    // PUBLIC TYPES
    typedef void volatile type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void const volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const volatile' type.

    // PUBLIC TYPES
    typedef void const volatile type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

#endif

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
