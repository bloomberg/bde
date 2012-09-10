// bslmf_addvolatile.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDVOLATILE
#define INCLUDED_BSLMF_ADDVOLATILE

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

#ifndef INCLUDED_BSLMF_ISVOLATILE
#include <bslmf_isvolatile.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE, bool ADD_VOLATILE>
struct AddVolatile_Imp
{
    typedef TYPE volatile Type;
};

template <typename TYPE>
struct AddVolatile_Imp<TYPE, false>
{
    typedef TYPE Type;
};

}
}

namespace bsl {

template <typename TYPE>
struct add_volatile
{
    typedef typename BloombergLP::bslmf::AddVolatile_Imp<
                            TYPE,
                            !is_reference<TYPE>::value
                            && !is_function<TYPE>::value
                            && !is_volatile<TYPE>::value>::Type type;
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
