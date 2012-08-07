// bslmf_hastrivialdefaultconstructor.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_HASTRIVIALDEFAULTCONSTRUCTOR
#define INCLUDED_BSLMF_HASTRIVIALDEFAULTCONSTRUCTOR

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
                        // struct HasTrivialDefaultConstructor
                        // ========================

template <class TYPE>
struct HasTrivialDefaultConstructor :
    integer_constant<bool,
                     IsFundamental<TYPE>::value
                  || IsEnum<TYPE>::value
                  || DetectNestedTrait<TYPE, HasTrivialDefaultConstructor>::value>
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bit-wise EqualityComparable.  If 'HasTrivialDefaultConstructor<TYPE>' is derived
    // from 'true_type' then 'TYPE' is bit-wise EqualityComparable.  Otherwise, bit-wise
    // moveability cannot be inferred for 'TYPE'.  This trait can be
    // associated with a bit-wise EqualityComparable user-defined class by specializing
    // this class or by using the 'BSLMF_DECLARE_NESTED_TRAIT' macro.
};

template <class TYPE>
struct HasTrivialDefaultConstructor<const TYPE> : HasTrivialDefaultConstructor<TYPE>::type
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <class TYPE>
struct HasTrivialDefaultConstructor<volatile TYPE> : HasTrivialDefaultConstructor<TYPE>::type
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <class TYPE>
struct HasTrivialDefaultConstructor<const volatile TYPE> : HasTrivialDefaultConstructor<TYPE>::type
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

template <class TYPE>
struct HasTrivialDefaultConstructor<TYPE*> : bsl::true_type
{
    // Specialization of that associates the 'HasTrivialDefaultConstructor' trait with
    // all pointer types.
};

template <class TYPE>
struct HasTrivialDefaultConstructor<TYPE&> : bsl::false_type
{
    // Specialization of that prevents associating the 'HasTrivialDefaultConstructor'
    // trait with any reference type.
};

}  // close package namespace

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLMF_HASTRIVIALDEFAULTCONSTRUCTOR)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

