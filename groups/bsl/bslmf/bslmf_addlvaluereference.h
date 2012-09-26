// bslmf_addlvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#define INCLUDED_BSLMF_ADDLVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to lvalue reference.
//
//@CLASSES:
//  bsl::add_lvalue_reference: standard meta-function for transforming type
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_lvalue_reference', which may be used to transform a type to a
// its lvalue reference type.
//
// 'bsl::add_lvalue_reference' meets the requirements of the
// 'add_lvalue_reference' template defined in the C++11 standard
// [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform to Lvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a couple of types to lvalue reference
// types.
//
// Now, we instantiate the 'bsl::add_lvalue_reference' template for these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
//..
//  assert(true ==
//        (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int&>::value));
//  assert(false ==
//        (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int >::value));
//  assert(true ==
//        (bsl::is_same<bsl::add_lvalue_reference<int&>::type,  int&>::value));
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true ==
//        (bsl::is_same<bsl::add_lvalue_reference<int&&>::type, int&>::value));
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

namespace bsl {

template <typename TYPE>
struct add_lvalue_reference
{
    typedef TYPE& type;
};

template <typename TYPE>
struct add_lvalue_reference<TYPE&>
{
    typedef TYPE& type;
};

#define BSL_DEFINE_ADD_LVALUE_REFERENCE(TYPE, REF_TYPE) \
template <>                                             \
struct add_lvalue_reference<TYPE>                       \
{                                                       \
    typedef REF_TYPE type;                              \
}                                                       \

BSL_DEFINE_ADD_LVALUE_REFERENCE(void, void);
BSL_DEFINE_ADD_LVALUE_REFERENCE(void const, void const);
BSL_DEFINE_ADD_LVALUE_REFERENCE(void volatile, void volatile);
BSL_DEFINE_ADD_LVALUE_REFERENCE(void const volatile, void const volatile);

#undef BSL_DEFINE_ADD_LVALUE_REFERENCE

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct add_lvalue_reference<TYPE&&>
{
    typedef TYPE& type;
};

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
