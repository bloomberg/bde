// bslmf_islvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#define INCLUDED_BSLMF_ISLVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_lvalue_reference : false_type
{};

template <typename TYPE>
struct is_lvalue_reference<TYPE &> : true_type
{};

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
