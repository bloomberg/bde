// bsls_types.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_TYPES
#define INCLUDED_BSLS_TYPES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a consistent interface for platform-dependent types.
//
//@CLASSES:
//  bsls::Types: namespace for platform-neutral type names
//
//@AUTHOR: Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a namespace for a set of 'typedef's
// that provide a stable, portable interface to platform-dependent types.  In
// particular, this component supplies portable typenames for signed and
// unsigned 64-bit integers ('bsls::Types::Int64' and 'bsls::Types::Uint64',
// respectively), as well as the preferred integral type denoting the number of
// elements in a container, and the number of bytes in a single block of memory
// supplied by an allocator ('bsls::Types::size_type').
//
///Usage
///-----
// The following illustrates how some of the types supplied by this component
// might be used.
//
// 'bsls::Types::Int64' and 'bsls::Types::Uint64' identify the preferred
// fundamental types denoting signed and unsigned 64-bit integers,
// respectively:
//..
//  bsls::Types::Uint64 stimulus = 787000000000ULL;
//..
// Clients can use these types in the same way as an 'int'.  Clients can also
// mix usage with other fundamental integral types:
//..
//  bsls::Types::Uint64 nationalDebt = 1000000000000ULL;
//  nationalDebt += stimulus;
//
//  unsigned int deficitReduction = 1000000000;
//  nationalDebt -= deficitReduction;
//
//  std::cout << "National Debt Level: " << nationalDebt << std::endl;
//..
// 'bsls::Types::size_type' identifies the preferred integral type denoting the
// number of elements in a container, and the number of bytes in a single block
// of memory supplied by an allocator.  For example, a typical use is as a
// 'typedef' in an STL container:
//..
//  class vector {
//
//      // ...
//
//    public:
//      typedef bsls::Types::size_type size_type;
//
//      // ...
//  };
//..
// Note that Standard Library facilities that work with numeric types can be
// used with 'bsls::Types' as well.  For example, the following code finds out
// some facts about 'bsls::Types::Int64' in a platform-independent way:
//..
//  std::cout << "Min Int64 value: "
//            << std::numeric_limits<bsls::Types::Int64>::min() << std::endl
//            << "Max Int64 value: "
//            << std::numeric_limits<bsls::Types::Int64>::max() << std::endl;
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bsls {

                          // ============
                          // struct Types
                          // ============

struct Types{
    // Provide a namespace for a suite of 'typedef's that encapsulate
    // platform-dependent types.

    // TYPES
#ifdef BSLS_PLATFORM_CPU_64_BIT
    typedef std::ptrdiff_t size_type;
#else
    typedef int            size_type;
#endif
        // The alias 'size_type' refers to the preferred type for denoting a
        // number of elements in either 'bslma' allocators or container types.
        // Note that this type is signed, as negative values may make sense in
        // certain contexts.  Also note that the standard-compliant allocators
        // (e.g., 'bslstl::Allocator' and 'std::allocator') use an *unsigned*
        // size type, but that is fine because they also have a mechanism
        // ('max_size') to determine overflows resulting from converting from
        // one size type to the other.

    typedef std::size_t    UintPtr;
    typedef std::ptrdiff_t IntPtr;
        // The aliases 'UintPtr' and 'IntPtr' are guaranteed to have the same
        // size as pointers.

#ifdef BSLS_PLATFORM_OS_WINDOWS
    typedef          __int64 Int64;
    typedef unsigned __int64 Uint64;
#else
    typedef          long long Int64;
    typedef unsigned long long Uint64;
#endif
        // The aliases 'Int64' and 'Uint64' stand for the appropriate types
        // that define signed and unsigned 64-bit integers, respectively, for
        // the appropriate supported platforms.
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bsls::Types bsls_Types;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
