// bsls_platformutil.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_PLATFORMUTIL
#define INCLUDED_BSLS_PLATFORMUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide consistent interface for platform-dependent functionality.
//
//@INTERNAL_DEPRECATED: Use 'bsls_alignmentutil', 'bsls_byteorder',
// 'bsls_platform', and 'bsls_types' instead.
//
//@CLASSES:
//  bsls::PlatformUtil: namespace for platform-neutral type names and API
//
//@SEE_ALSO: bsls_platform, bsls_types
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for a set of 'typedef's and
// functions that provide a stable, portable interface to platform-dependent
// functionality.  In particular, this component supplies portable typenames
// for signed and unsigned 64-bit integers, and provides compile-time
// preprocessor macros that characterize the "endian-ness" of the underlying
// processor on the current platform.  Runtime functionality to return the
// "endian-ness", and to round up to a multiple of the maximum required
// alignment for the processor, are also provided.
//
///Usage
///-----
// The following illustrates how some of the types and functions supplied by
// this component might be used.
//
///Types
///- - -
// 'bsls::PlatformUtil::Int64' and 'bsls::PlatformUtil::Uint64' identify the
// preferred fundamental types denoting signed and unsigned 64-bit integers,
// respectively:
//..
//  bsls::PlatformUtil::Uint64 stimulus = 787000000000ULL;
//..
// Clients can use these types in the same way as an 'int'.  Clients can also
// mix usage with other fundamental integral types:
//..
//  bsls::PlatformUtil::Uint64 nationalDebt = 1000000000000ULL;
//  nationalDebt += stimulus;
//
//  unsigned int deficitReduction = 1000000000;
//  nationalDebt -= deficitReduction;
//
//  std::cout << "National Debt Level: " << nationalDebt << std::endl;
//..
// 'bsls::PlatformUtil::size_type' identifies the preferred integral type
// denoting the number of elements in a container, and the number of bytes in a
// single block of memory supplied by an allocator.  For example, a typical use
// is as a 'typedef' in an STL container:
//..
//  class vector {
//
//      // ...
//
//    public:
//      typedef bsls::PlatformUtil::size_type size_type;
//
//      // ...
//  };
//..
//
///Functions and Macros
/// - - - - - - - - - -
// The functions:
//..
//  bool bsls::PlatformUtil::isLittleEndian();
//  bool bsls::PlatformUtil::isBigEndian();
//..
// encapsulate the capability of determining whether a machine is big- or
// little-endian across all supported platforms.  In addition, certain
// compile-time constants are also provided as preprocessor macros to
// facilitate conditional compilation:
//..
//  BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
//  BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN
//..
// These functions and macros are useful for writing platform-independent code,
// such as a function that converts the bytes in a 'short' to network byte
// order (which is consistent with big-endian):
//..
//  short convertToNetworkByteOrder(short input)
//      // Return the specified 'input' in network byte order.
//  {
//  #ifdef BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
//      return input;
//  #else
//      return ((input >> 8) & 0xFF) | ((input & 0xFF) << 8);
//  #endif
//  }
//..
// Note that in the above usage example, either the macros or the functions can
// be used to test whether a platform is big- or little-endian.

#ifdef BDE_OMIT_TRANSITIONAL // DEPRECATED
#error "bsls_platformutil is deprecated"
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bsls {

                          // ===================
                          // struct PlatformUtil
                          // ===================

struct PlatformUtil {
    // Provide a namespace for a suite of 'typedef's and pure procedures that
    // encapsulate, platform-dependent types and APIs.

    // TYPES
    typedef Types::size_type size_type;
        // The alias 'size_type' refers to the preferred type for denoting a
        // number of elements in either 'bslma' allocators or container types.
        // Note that this type is signed, as negative values may make sense in
        // certain contexts.  Also note that the standard-compliant allocators
        // (e.g., 'bslstl_allocator' and 'std::allocator') use an *unsigned*
        // size type, but that is fine because they also have a mechanism
        // ('max_size') to determine overflows resulting from converting from
        // one size type to the other.
        //
        // DEPRECATED: Use 'Types::size_type' instead.

    typedef Types::UintPtr UintPtr;
    typedef Types::IntPtr  IntPtr;
        // The aliases 'UintPtr' and 'IntPtr' are guaranteed to have the same
        // size as pointers.
        //
        // DEPRECATED: Use 'Types::UintPtr' and 'Types::IntPtr' instead.

    typedef Types::Int64  Int64;
    typedef Types::Uint64 Uint64;
        // The aliases 'Int64' and 'Uint64' stand for whatever type 'Types'
        // implements for the appropriate supported platforms.
        //
        // DEPRECATED: Use 'Types::Int64' and 'Types::Uint64' instead.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    typedef AlignmentUtil::MaxAlignedType MaxAlign;
        // The alias 'MaxAlign' refers to a type that is maximally-aligned on
        // the current platform.
        //
        // DEPRECATED: Use 'AlignmentUtil::MaxAlignedType' instead.
#endif

    // CLASS METHODS
    static bool isBigEndian();
        // Return 'true' if this platform is "big-endian", and 'false'
        // otherwise.  Note that "big-endian" (i.e., the most significant byte
        // of data at the lowest byte address) is consistent with network byte
        // order.
        //
        // DEPRECATED: Use preprocessor macro 'BSLS_PLATFORM__IS_BIG_ENDIAN'
        // defined in 'bsls_platform' instead.

    static bool isLittleEndian();
        // Return 'true' if this platform is "little-endian", and 'false'
        // otherwise.  Note that "little-endian" (i.e., the least significant
        // byte of data at the lowest byte address) is inconsistent with
        // network byte order.
        //
        // DEPRECATED: Use preprocessor macro
        // 'BSLS_PLATFORMUTIL__IS_BIG_ENDIAN' defined in 'bsls_platform'
        // instead.

    static int roundUpToMaximalAlignment(int size);
        // Return the specified 'size' (in bytes) rounded up to the smallest
        // integral multiple of the maximum alignment.  The behavior is
        // undefined unless '0 <= size'.
        //
        // DEPRECATED: Use 'AlignmentUtil::roundUpToMaximalAlignment' instead.
};

}  // close package namespace

// ============================================================================
//                 COMPILE-TIME CONSTANT PRE-PROCESSOR MACROS
// ============================================================================

// The following preprocessor macros are DEPRECATED.  Please use their
// replacements in 'bsls_byteorder' and 'bsls_platform' instead.

#if defined(BSLS_PLATFORM__CPU_X86_64)
    #define BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN BSLS_PLATFORM__IS_LITTLE_ENDIAN
        // DEPRECATED: Use preprocessor macro 'BSLS_PLATFORM__IS_LITTLE_ENDIAN'
        // defined in 'bsls_platform' instead.
#endif

#if defined(BSLS_PLATFORM__CPU_X86)
    #define BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN \
                                                BSLS_PLATFORM__IS_LITTLE_ENDIAN
        // DEPRECATED: Use preprocessor macro 'BSLS_PLATFORM__IS_LITTLE_ENDIAN'
        // defined in 'bsls_platform' instead.
#endif

#if !defined(BSLS_PLATFORMUTIL__IS_LITTLE_ENDIAN)
   #define BSLS_PLATFORMUTIL__IS_BIG_ENDIAN BSLS_PLATFORM__IS_BIG_ENDIAN
       // DEPRECATED: Use preprocessor macro 'BSLS_PLATFORM__IS_BIG_ENDIAN'
       // defined in 'bsls_platform' instead.
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

#if defined(BSLS_PLATFORMUTIL__IS_BIG_ENDIAN)
#define BSLS_PLATFORMUTIL__HTONL(x) (x)
#define BSLS_PLATFORMUTIL__HTONS(x) (x)
#define BSLS_PLATFORMUTIL__NTOHL(x) (x)
#define BSLS_PLATFORMUTIL__NTOHS(x) (x)
    // DEPRECATED: Use preprocessor macros 'BSLS_BYTEORDER_*TO*' defined in
    // 'bsls_byteorder' instead.
#else
    // Use built-in if using gcc 4.3.

#if defined(BSLS_PLATFORM__CMP_GNU) \
 && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300
#define BSLS_PLATFORMUTIL__NTOHL(x) (unsigned) __builtin_bswap32(x)
#else
#define BSLS_PLATFORMUTIL__NTOHL(x) \
                   ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
                   (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#endif
#define BSLS_PLATFORMUTIL__HTONL(x) BSLS_PLATFORMUTIL__NTOHL(x)
#define BSLS_PLATFORMUTIL__NTOHS(x) \
                                    ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
#define BSLS_PLATFORMUTIL__HTONS(x) BSLS_PLATFORMUTIL__NTOHS(x)
#endif

#endif

namespace bsls {

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -------------------
                          // struct PlatformUtil
                          // -------------------

// CLASS METHODS
inline
bool PlatformUtil::isLittleEndian()
{
#if defined(BSLS_PLATFORM__IS_LITTLE_ENDIAN)
    return BSLS_PLATFORM__IS_LITTLE_ENDIAN;
#else
    return false;
#endif
}

inline
bool PlatformUtil::isBigEndian()
{
#if defined(BSLS_PLATFORM__IS_BIG_ENDIAN)
    return BSLS_PLATFORM__IS_BIG_ENDIAN;
#else
    return false;
#endif
}

inline
int PlatformUtil::roundUpToMaximalAlignment(int size)
{
    enum { BSLS_MAX_ALIGN = AlignmentUtil::BSLS_MAX_ALIGNMENT };
    return ((size + BSLS_MAX_ALIGN - 1) / BSLS_MAX_ALIGN) * BSLS_MAX_ALIGN;
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bsls::PlatformUtil bsls_PlatformUtil;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
