// bslmf_isfloatingpoint.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#define INCLUDED_BSLMF_ISFLOATINGPOINT

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

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsFloatingPoint_Imp : bsl::false_type
{};

template <> struct IsFloatingPoint_Imp<float> : bsl::true_type {};
template <> struct IsFloatingPoint_Imp<double> : bsl::true_type {};
template <> struct IsFloatingPoint_Imp<long double> : bsl::true_type {};

}
}

namespace bsl {

template <typename TYPE>
struct is_floating_point
    : BloombergLP::bslmf::IsFloatingPoint_Imp<
                                typename remove_cv<TYPE>::type>::type
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
