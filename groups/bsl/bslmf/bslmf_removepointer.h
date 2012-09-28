// bslmf_removepointer.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#define INCLUDED_BSLMF_REMOVEPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct RemovePointer_Imp
{
    typedef TYPE Type;
};

template <typename TYPE>
struct RemovePointer_Imp<TYPE *>
{
    typedef TYPE Type;
};

}
}

namespace bsl {

template <typename TYPE>
struct remove_pointer
{
    typedef typename BloombergLP::bslmf::RemovePointer_Imp<
                typename remove_cv<TYPE>::type>::Type
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
