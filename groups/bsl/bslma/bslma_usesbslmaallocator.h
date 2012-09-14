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

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class UsesBslmaAllocator
                        // ========================

template <class TYPE, bool IS_NESTED>
struct UsesBslmaAllocator_Imp : bsl::integer_constant<bool, IS_NESTED>
{
};

template <class TYPE>
struct UsesBslmaAllocator_Imp<TYPE, false> : bsl::false_type
{
private:
    typedef struct UniqueType {
        // A class convertible from this type must have a templated
        // constructor or a 'void*' which makes it convertible from EVERY
        // pointer type.
    } *UniquePtrType;

    enum {
        // If a pointer to 'Allocator' is convertible to 'T', then 'T' has a
        // non-explcit constructor taking an allocator.
        BSLMA_POINTER_CTOR = bslmf::IsConvertible<Allocator*, TYPE>::value,

        // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
        // that ANY POINTER is convertible to 'T'.
        ANY_POINTER_CTOR = bslmf::IsConvertible<UniqueType*, TYPE>::VALUE,

        // 'SNIFFED_BSLMA_IDIOM' will be true if the old traits mechanism
        // would have detected an idiomatic type through trait sniffing.
        SNIFFED_BSLMA_IDIOM = BSLMA_POINTER_CTOR && ! ANY_POINTER_CTOR
    };

public:
    // If 'UsesBslmaAllocator' is not specialized for 'TYPE' and is not
    // declared as a nested trait within 'TYPE', then return 'false_type'.
    // However, the following static assertion will fire if the user makes the
    // dangerous assumption that having a conversion from 'Allocator*' to
    // 'TYPE' will result in the 'UsesBslmaAllocator' trait being
    // automatically associated with 'TYPE'; such sniffing is no longer
    // supported.
    BSLMF_ASSERT(! SNIFFED_BSLMA_IDIOM);
};

template <class TYPE>
struct UsesBslmaAllocator : UsesBslmaAllocator_Imp<TYPE,
    bslmf::DetectNestedTrait<TYPE, UsesBslmaAllocator>::VALUE>::type
{
    // This metafunction is derived from 'true_type' if 'TYPE' adheres to the
    // 'bslma' allocator usage idiom and 'false_type' otherwise.  Note that
    // this trait must be explicitly associated with a type in order for this
    // metafunction to return true; simply havng a constructor that implicitly
    // converts 'bslma::Allocator*' to 'TYPE' is no longer sufficient for
    // considering a type follow the idiom.
};

template <class TYPE>
struct UsesBslmaAllocator<const TYPE> : UsesBslmaAllocator<TYPE>
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<volatile TYPE> : UsesBslmaAllocator<TYPE>
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<const volatile TYPE> : UsesBslmaAllocator<TYPE>
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

// FREE OPERATORS

}  // close package namespace

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS

// CREATORS

// MANIPULATORS

// ACCESSORS

// FREE OPERATORS

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_USESBSLMAALLOCATOR)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
