// bslmf_isrvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#define INCLUDED_BSLMF_ISRVALUEREFERENCE

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

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_rvalue_reference : false_type
{};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct is_rvalue_reference<TYPE &&> : true_type
{};

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
