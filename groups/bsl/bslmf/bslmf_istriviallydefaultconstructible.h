// bslmf_istriviallydefaultconstructible.h                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_trivially_default_constructible;

}

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsTriviallyDefaultConstructible_Imp :
    integer_constant<bool,
                     !bsl::is_reference<TYPE>::value
                     && (  IsFundamental<TYPE>::value
                        || IsEnum<TYPE>::value
                        || bsl::is_pointer<TYPE>::value
                        || bslmf::IsPointerToMember<TYPE>::value
                        || DetectNestedTrait<TYPE,
                              bsl::is_trivially_default_constructible>::value)>
{};

}
}

namespace bsl {

template <typename TYPE>
struct is_trivially_default_constructible
    : BloombergLP::bslmf::IsTriviallyDefaultConstructible_Imp<
                                                typename remove_cv<TYPE>::type>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is trivially default constructible.  If
    // 'is_trivially_default_constructible<TYPE>' is derived from 'true_type'
    // then 'TYPE' is trivially default constructible.
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

