// bslmf_addlvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#define INCLUDED_BSLMF_ADDLVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

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

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct add_lvalue_reference<TYPE&>
{
    typedef TYPE& type;
};

#else  // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct add_lvalue_reference<TYPE&&>
{
    typedef TYPE& type;
};

#endif

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
