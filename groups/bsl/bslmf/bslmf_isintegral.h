// bslmf_isintegral.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISINTEGRAL
#define INCLUDED_BSLMF_ISINTEGRAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsIntegral_Imp : bsl::false_type
{};

template <> struct IsIntegral_Imp<bool> : bsl::true_type {};
template <> struct IsIntegral_Imp<char> : bsl::true_type {};
template <> struct IsIntegral_Imp<wchar_t> : bsl::true_type {};
template <> struct IsIntegral_Imp<signed char> : bsl::true_type {};
template <> struct IsIntegral_Imp<unsigned char> : bsl::true_type {};
template <> struct IsIntegral_Imp<short> : bsl::true_type {};
template <> struct IsIntegral_Imp<unsigned short> : bsl::true_type {};
template <> struct IsIntegral_Imp<int> : bsl::true_type {};
template <> struct IsIntegral_Imp<unsigned int> : bsl::true_type {};
template <> struct IsIntegral_Imp<long int> : bsl::true_type {};
template <> struct IsIntegral_Imp<unsigned long int> : bsl::true_type {};
template <> struct IsIntegral_Imp<bsls::Types::Int64> : bsl::true_type {};
template <> struct IsIntegral_Imp<bsls::Types::Uint64> : bsl::true_type {};

}
}

namespace bsl {

template <typename TYPE>
struct is_integral
    : BloombergLP::bslmf::IsIntegral_Imp<typename remove_cv<TYPE>::type>::type
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
