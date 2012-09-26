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
//  bsl::add_rvalue_reference: standard meta-function for transforming type
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_rvalue_reference', which may be used to transform a type to its
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
// Suppose that we want to transform a couple of types to rvalue reference
// types.
//
// Now, we instantiate the 'bsl::add_rvalue_reference' template for these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
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
// Note that rvalue is introduced in C++11 and may not be supported by all
// compilers.  Note also that according to 'reference collapsing' semantics
// [8.3.2], 'add_rvalue_reference' does not transform 'TYPE' to rvalue
// reference type if 'TYPE' is a lvalue reference type.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace bsl {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct add_rvalue_reference
{
    typedef TYPE&& type;
};

#define BSL_DEFINE_ADD_RVALUE_REFERENCE(TYPE, REF_TYPE) \
template <>                                             \
struct add_rvalue_reference<TYPE>                       \
{                                                       \
    typedef REF_TYPE type;                              \
}                                                       \

BSL_DEFINE_ADD_RVALUE_REFERENCE(void, void);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void const, void const);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void volatile, void volatile);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void const volatile, void const volatile);

#undef BSL_DEFINE_ADD_RVALUE_REFERENCE

#endif

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
