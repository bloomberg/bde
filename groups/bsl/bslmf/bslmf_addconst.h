// bslmf_addconst.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCONST
#define INCLUDED_BSLMF_ADDCONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONST
#include <bslmf_isconst.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE, bool ADD_CONST>
struct AddConst_Imp
{
    typedef TYPE const Type;
};

template <typename TYPE>
struct AddConst_Imp<TYPE, false>
{
    typedef TYPE Type;
};

}
}

namespace bsl {

template <typename TYPE>
struct add_const
{
    typedef typename BloombergLP::bslmf::AddConst_Imp<
                            TYPE,
                            !is_reference<TYPE>::value
                            && !is_function<TYPE>::value
                            && !is_const<TYPE>::value>::Type type;
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
