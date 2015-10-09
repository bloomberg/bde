// bslstp_util.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTP_UTIL
#define INCLUDED_BSLSTP_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for utility functions for STL functionality.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//  bslstp::Util: namespace for utility functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a class, 'bslstp::Util', that provides a
// namespace for utility functions used to implement STL functionality in the
// 'bslstp' package.
//
///Usage
///-----
// This component is for internal use only.

// Prevent 'bslstp' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstp_util.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bslstp {

                        // ==========
                        // class Util
                        // ==========

class Util {
    // Namespace for utility functions used to implement STL functionality.

    // PRIVATE TYPES
    template <class ALLOCATOR, int IS_BSLMA_ALLOC>
    struct AllocatorUtil {
        // Simplify template specializations and overloading.

        static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
            // Return the appropriate allocator for use when copy-constructing
            // a container.  'rhsAlloc' is intended to be the allocator from
            // the container being copied.  If 'isBslmaAlloc' is of type
            // 'bsl::true_type' then ignore 'rhsAlloc' and return the
            // default allocator.  Otherwise, return 'rhsAlloc' unchanged.
    };

    template <class ALLOCATOR>
    struct AllocatorUtil<ALLOCATOR, 0> {
        // Specialization for non-'bslma' allocators.

        static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
            // Return the appropriate allocator for use when copy-constructing
            // a container.  'rhsAlloc' is intended to be the allocator from
            // the container being copied.  If 'isBslmaAlloc' is of type
            // 'bsl::true_type' then ignore 'rhsAlloc' and return the
            // default allocator.  Otherwise, return 'rhsAlloc' unchanged.
    };

  public:
    // CLASS METHODS
    template <class ALLOCATOR>
    static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
        // Return the appropriate allocator for use when copy-constructing a
        // container.  'rhsAlloc' is intended to be the allocator from the
        // container being copied.  If the 'ALLOCATOR' type uses 'bslma'
        // allocator semantics, then ignore 'rhsAlloc' and return the default
        // allocator.  Otherwise, return 'rhsAlloc' unchanged.

    template <class CONTAINER, class QUICKSWAP_FUNC>
    static void swapContainers(CONTAINER&            c1,
                               CONTAINER&            c2,
                               const QUICKSWAP_FUNC& quickswap);
        // Swap the contents of containers 'c1' and 'c2', correctly handling
        // the case where the two containers have different allocator values.
        // The 'quickswap' argument is a binary function object that is called
        // to quickly swap two 'CONTAINER' objects with matching allocators.
        // (The 'quickswap' object does not need to check that the allocator
        // matched -- it is guaranteed by the caller.)  Strong exception
        // guarantee: if the allocator or copy constructor of either container
        // throws an exception, then the values of 'c1' and 'c2' are left
        // unchanged.  The 'quickswap' function must not throw an exception.
};

}  // close package namespace

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

               //------------------------------------------------
               // struct AllocatorUtil<ALLOCATOR, IS_BSLMA_ALLOC>
               //------------------------------------------------

// PRIVATE CLASS METHODS
template <class ALLOCATOR, int IS_BSLMA_ALLOC>
inline
ALLOCATOR bslstp::Util::AllocatorUtil<ALLOCATOR, IS_BSLMA_ALLOC>::
copyContainerAllocator(const ALLOCATOR&)
{
    return ALLOCATOR();
}

                     //-----------------------------------
                     // struct AllocatorUtil<ALLOCATOR, 0>
                     //-----------------------------------

// PRIVATE CLASS METHODS
template <class ALLOCATOR>
inline
ALLOCATOR bslstp::Util::AllocatorUtil<ALLOCATOR, 0>::
copyContainerAllocator(const ALLOCATOR& rhsAlloc)
{
    return rhsAlloc;
}

namespace bslstp {

                              //-----------
                              // class Util
                              //-----------

// CLASS METHODS
template <class ALLOCATOR>
inline
ALLOCATOR Util::copyContainerAllocator(const ALLOCATOR& rhsAlloc)
{
    typedef typename
        bsl::is_convertible<bslma::Allocator*,ALLOCATOR>::type IsBslma;

    return AllocatorUtil<ALLOCATOR, IsBslma::value>::copyContainerAllocator(
                                                                     rhsAlloc);
}

template <class CONTAINER, class QUICKSWAP_FUNC>
void Util::swapContainers(CONTAINER&            c1,
                          CONTAINER&            c2,
                          const QUICKSWAP_FUNC& quickswap)
{
    typedef typename CONTAINER::allocator_type allocator_type;
    allocator_type alloc1 = c1.get_allocator();
    allocator_type alloc2 = c2.get_allocator();

    if (alloc1 == alloc2) {
        quickswap(c1, c2);
    }
    else {
        // Create copies of c1 and c2 using each-other's allocators Exception
        // leaves originals untouched.

        CONTAINER c1copy(c1, alloc2);
        CONTAINER c2copy(c2, alloc1);

        // Now use bit-wise swap (no exceptions thrown).

        quickswap(c1, c2copy);
        quickswap(c2, c1copy);
    }
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslstp::Util bslstp_Util;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
