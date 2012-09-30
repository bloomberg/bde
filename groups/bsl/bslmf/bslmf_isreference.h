// bslmf_isreference.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISREFERENCE
#define INCLUDED_BSLMF_ISREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for reference types.
//
//@CLASSES:
//  bsl::is_reference: standard meta-function for reference types
//
//@SEE_ALSO:
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_reference',
// which may be used to query whether a type is a (lvalue or rvalue) reference
// type.
//
// 'bsl::is_reference' meets the requirements of the 'is_reference' template
// defined in the C++11 standard [meta.unary.comp].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Reference Types
///- - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a (lvalue or
// rvalue) reference type.
//
// Now, we instantiate the 'bsl::is_reference' template for a non-reference
// type, a lvalue reference type, and a rvalue reference type, and assert the
// 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_reference<int>::value);
//  assert(true  == bsl::is_reference<int&>::value);
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  == bsl::is_reference<int&&>::value);
//#endif
//..
// Note that rvalue is introduced in C++11 and may not be supported by all
// compilers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#include <bslmf_islvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#include <bslmf_isrvaluereference.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_reference : integral_constant<bool,
                                        is_lvalue_reference<TYPE>::value
                                        || is_rvalue_reference<TYPE>::value>
{};

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
