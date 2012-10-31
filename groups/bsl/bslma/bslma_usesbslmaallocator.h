// bslma_usesbslmaallocator.h                  -*-C++-*-
#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#define INCLUDED_BSLMA_USESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction that indicates the use of bslma allocators
//
//@CLASSES: bslma::UsesBslmaAllocator<TYPE>
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#include <bslmf_removepointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class UsesBslmaAllocator
                        // ========================

template <typename TYPE, bool IS_NESTED>
struct UsesBslmaAllocator_Imp
{
    typedef bsl::integral_constant<bool, IS_NESTED> Type;
};

template <typename TYPE>
struct UsesBslmaAllocator_Imp<TYPE, false>
{
private:
    struct UniqueType {
        // A class convertible from this type must have a templated
        // constructor or a 'void*' which makes it convertible from EVERY
        // pointer type.
    };

    // Detect if 'TYPE' is 'Allocator*' type.
    static const bool IS_BSLMA_POINTER
        = bsl::is_same<
                Allocator,
                typename bsl::remove_cv<
                    typename bsl::remove_pointer<TYPE>::type>::type>::value;

    // If a pointer to 'Allocator' is convertible to 'T', then 'T' has a
    // non-explcit constructor taking an allocator.
    static const bool BSLMA_POINTER_CTOR
        = bsl::is_convertible<Allocator *, TYPE>::value;

    // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
    // that ANY POINTER is convertible to 'T'.
    static const bool ANY_POINTER_CTOR
        = bsl::is_convertible<UniqueType *, TYPE>::value;

public:
    typedef bsl::integral_constant<bool,
                                   !IS_BSLMA_POINTER
                                   && BSLMA_POINTER_CTOR
                                   && !ANY_POINTER_CTOR>
        Type;
};

template <typename TYPE>
struct UsesBslmaAllocator
    : UsesBslmaAllocator_Imp<
        TYPE,
        bslmf::DetectNestedTrait<TYPE, UsesBslmaAllocator>::value>::Type::type
{
    // This metafunction is derived from 'true_type' if 'TYPE' adheres to the
    // 'bslma' allocator usage idiom and 'false_type' otherwise.  Note that
    // this trait must be explicitly associated with a type in order for this
    // metafunction to return true; simply havng a constructor that implicitly
    // converts 'bslma::Allocator*' to 'TYPE' is no longer sufficient for
    // considering a type follow the idiom.
};

template <typename TYPE>
struct UsesBslmaAllocator<const TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <typename TYPE>
struct UsesBslmaAllocator<volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <typename TYPE>
struct UsesBslmaAllocator<const volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
