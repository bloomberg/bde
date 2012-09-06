// bslmf_removevolatile.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#define INCLUDED_BSLMF_REMOVEVOLATILE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

template <typename TYPE>
struct remove_volatile
{
    typedef TYPE type;
};

template <typename TYPE>
struct remove_volatile<TYPE volatile>
{
    typedef TYPE type;
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
