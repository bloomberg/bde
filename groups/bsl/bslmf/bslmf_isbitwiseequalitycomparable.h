// bslmf_isbitwiseequalitycomparable.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE

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

namespace BloombergLP {

namespace bslmf {

template <typename TYPE>
struct IsBitwiseEqualityComparable;

template <typename TYPE>
struct IsBitwiseEqualityComparable_Imp
    : bsl::integer_constant<bool,
                            !bsl::is_reference<TYPE>::value
                         && (  IsFundamental<TYPE>::value
                            || IsEnum<TYPE>::value
                            || bsl::is_pointer<TYPE>::value
                            || bslmf::IsPointerToMember<TYPE>::value
                            || DetectNestedTrait<TYPE,
                                          IsBitwiseEqualityComparable>::value)>
{};

                     // ==================================
                     // struct IsBitwiseEqualityComparable
                     // ==================================

template <typename TYPE>
struct IsBitwiseEqualityComparable
    : IsBitwiseEqualityComparable_Imp<typename bsl::remove_cv<TYPE>::type>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bit-wise EqualityComparable.  If
    // 'IsBitwiseEqualityComparable<TYPE>' is derived from 'true_type' then
    // 'TYPE' is bit-wise EqualityComparable.  Otherwise, bit-wise moveability
    // cannot be inferred for 'TYPE'.  This trait can be associated with a
    // bit-wise EqualityComparable user-defined class by specializing this
    // class or by using the 'BSLMF_DECLARE_NESTED_TRAIT' macro.
};

}  // close package namespace

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLMF_ISBITWISECOPYABLE)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

