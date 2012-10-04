// bslmf_isrvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#define INCLUDED_BSLMF_ISRVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for rvalue reference types.
//
//@CLASSES:
//  bsl::is_rvalue_reference: standard meta-function for rvalue reference types
//
//@SEE_ALSO:
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_rvalue_reference', that may be used to query whether a type is an
// rvalue reference type.
//
// 'bsl::is_rvalue_reference' meets the requirements of the
// 'is_rvalue_reference' template defined in the C++11 standard
// [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are rvalue reference
// types.
//
// Now, we instantiate the 'bsl::is_rvalue_reference' template for both a
// non-reference type and an rvalue reference type, and assert the 'value'
// static data member of each instantiation:
//..
//  assert(false == bsl::is_rvalue_reference<int>::value);
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  == bsl::is_rvalue_reference<int&&>::value);
//#endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standard,
// and may not be supported by all compilers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace bsl {

                          // ==========================
                          // struct is_rvalue_reference
                          // ==========================

template <typename TYPE>
struct is_rvalue_reference : false_type
{
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is an (cv-qualified) rvalue reference type.
    // This generic default template derives from 'bsl::false_type'.  A
    // template specialization is provided (below) that derives from
    // 'bsl::true_type'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct is_rvalue_reference<TYPE &&> : true_type
{
    // This partial specialization of 'is_rvalue_reference' derives from
    // 'bsl::true_type' for when the (template parameter) 'TYPE' is an rvalue
    // reference type.
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
