// bslmf_haspointersemantics.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_HASPOINTERSEMANTICS
#define INCLUDED_BSLMF_HASPOINTERSEMANTICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type trait for pointer semantics.
//
//@CLASSES:
//  bslmf::HasPointerSemantics: meta-function for determining pointer-like types
//
//@SEE_ALSO:

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

namespace BloombergLP {

namespace bslmf {

template <typename TYPE>
struct HasPointerSemantics
    : bsl::integer_constant<bool,
                            bsl::is_pointer<TYPE>::value
                            || DetectNestedTrait<TYPE, HasPointerSemantics>::value>
    //
{
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
