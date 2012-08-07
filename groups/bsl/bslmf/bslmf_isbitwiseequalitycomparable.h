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

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // ========================
                        // struct IsBitwiseEqualityComparable
                        // ========================

template <class TYPE>
struct IsBitwiseEqualityComparable :
    integer_constant<bool,
                     IsFundamental<TYPE>::value
                  || IsEnum<TYPE>::value
                  || DetectNestedTrait<TYPE, IsBitwiseEqualityComparable>::value>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bit-wise EqualityComparable.  If 'IsBitwiseEqualityComparable<TYPE>' is derived
    // from 'true_type' then 'TYPE' is bit-wise EqualityComparable.  Otherwise, bit-wise
    // moveability cannot be inferred for 'TYPE'.  This trait can be
    // associated with a bit-wise EqualityComparable user-defined class by specializing
    // this class or by using the 'BSLMF_DECLARE_NESTED_TRAIT' macro.
};

template <class TYPE>
struct IsBitwiseEqualityComparable<const TYPE> : IsBitwiseEqualityComparable<TYPE>::type
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <class TYPE>
struct IsBitwiseEqualityComparable<volatile TYPE> : IsBitwiseEqualityComparable<TYPE>::type
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <class TYPE>
struct IsBitwiseEqualityComparable<const volatile TYPE> : IsBitwiseEqualityComparable<TYPE>::type
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

template <class TYPE>
struct IsBitwiseEqualityComparable<TYPE*> : bsl::true_type
{
    // Specialization of that associates the 'IsBitwiseEqualityComparable' trait with
    // all pointer types.
};

template <class TYPE>
struct IsBitwiseEqualityComparable<TYPE&> : bsl::false_type
{
    // Specialization of that prevents associating the 'IsBitwiseEqualityComparable'
    // trait with any reference type.
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

