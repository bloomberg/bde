// bslmf_addrvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#define INCLUDED_BSLMF_ADDRVALUEREFERENCE

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
BSL_DEFINE_ADD_RVALUE_REFERENCE(const void, const void);
BSL_DEFINE_ADD_RVALUE_REFERENCE(volatile void, volatile void);
BSL_DEFINE_ADD_RVALUE_REFERENCE(const volatile void, const volatile void);

#undef BSL_DEFINE_ADD_RVALUE_REFERENCE

#endif

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
