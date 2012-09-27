// bslmf_removecv.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECV
#define INCLUDED_BSLMF_REMOVECV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECONST
#include <bslmf_removeconst.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#include <bslmf_removevolatile.h>
#endif

namespace bsl {

template <typename TYPE>
struct remove_cv
{
    typedef typename remove_const<typename remove_volatile<TYPE>::type>::type
        type;
};

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
