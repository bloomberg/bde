// bslmf_ismemberobjectpointer.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#define INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER

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

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#include <bslmf_ismemberfunctionpointer.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsPointerToMemberData_Imp : bsl::false_type {
};

template <typename TYPE, typename CLASS>
struct IsPointerToMemberData_Imp<TYPE CLASS::*> : bsl::true_type {
};

}
}

namespace bsl {

template <typename TYPE>
struct is_member_object_pointer
    : integer_constant<bool,
                       BloombergLP::bslmf::IsPointerToMemberData_Imp<
                           typename remove_cv<TYPE>::type>::value
                       && !is_member_function_pointer<TYPE>::value
                       && !is_reference<TYPE>::value>
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
