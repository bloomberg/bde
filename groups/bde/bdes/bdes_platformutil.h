// bdes_platformutil.h             -*-C++-*-
#ifndef INCLUDED_BDES_PLATFORMUTIL
#define INCLUDED_BDES_PLATFORMUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Encapsulate platform-dependent operations behind stable interface.
//
//@DEPRECATED: Use 'bsls_platformutil' instead.
//
//@CLASSES:
//   bdes_PlatformUtil: namespace for platform-dependent functionality
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a namespace for a collection of
// typedefs and pure procedures that would otherwise have platform-dependent
// interfaces.  By platform, we mean the combination of the compiler, operating
// system, and hardware used.  For example:
//..
//     bdes_PlatformUtil::Int64    and    bdes_PlatformUtil::Uint64
//..
// identify the preferred fundamental types denoting signed and unsigned
// 64-bit integers, respectively, and:
//..
//     bdes_PlatformUtil::size_type
//..
// identifies the preferred integral type denoting a number of elements in
// allocators and containers.  Similarly, the functions:
//..
//   int bdes_PlatformUtil::isLittleEndian();
//   int bdes_PlatformUtil::isBigEndian();
//..
// encapsulate the capability of determining whether a machine is big- or
// little-endian across all supported platforms.  In addition, certain
// compile-time constants are also provided as preprocessor macros to
// facilitate conditional compilation.
//..
//   BDES_PLATFORMUTIL_IS_BIG_ENDIAN
//   BDES_PLATFORMUTIL_IS_LITTLE_ENDIAN
//..
// Another set of macros define macros replacing the standard 'htonl', 'htons',
// 'ntohs', 'ntohl' which does not require including any system header:
//..
//   BDES_PLATFORMUTIL_HTONL(x)
//   BDES_PLATFORMUTIL_HTONS(x)
//   BDES_PLATFORMUTIL_NTOHL(x)
//   BDES_PLATFORMUTIL_NTOHS(x)
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_BYTEORDER
#include <bsls_byteorder.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

typedef bsls::PlatformUtil bdes_PlatformUtil;
    // Provide a namespace for the interface to a suite of encapsulated,
    // platform-dependent types and operations.

}  // close namespace BloombergLP

#ifdef BSLS_PLATFORMUTIL_IS_LITTLE_ENDIAN
#define BDES_PLATFORMUTIL_IS_LITTLE_ENDIAN BSLS_PLATFORMUTIL_IS_LITTLE_ENDIAN
#endif

#ifdef BSLS_PLATFORMUTIL_IS_BIG_ENDIAN
#define BDES_PLATFORMUTIL_IS_BIG_ENDIAN    BSLS_PLATFORMUTIL_IS_BIG_ENDIAN
#endif


#ifndef BDES_PLATFORMUTIL_HTONL
#define BDES_PLATFORMUTIL_HTONL(X)         BSLS_BYTEORDER_HTONL(X)
#endif

#ifndef BDES_PLATFORMUTIL_HTONS
#define BDES_PLATFORMUTIL_HTONS(X)         BSLS_BYTEORDER_HTONS(X)
#endif

#ifndef BDES_PLATFORMUTIL_NTOHL
#define BDES_PLATFORMUTIL_NTOHL(X)         BSLS_BYTEORDER_NTOHL(X)
#endif

#ifndef BDES_PLATFORMUTIL_NTOHS
#define BDES_PLATFORMUTIL_NTOHS(X)         BSLS_BYTEORDER_NTOHS(X)
#endif

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT

#ifdef BDES_PLATFORMUTIL_IS_BIG_ENDIAN
# define BDES_PLATFORMUTIL__IS_BIG_ENDIAN BDES_PLATFORMUTIL_IS_BIG_ENDIAN
#endif
#ifdef BDES_PLATFORMUTIL_IS_LITTLE_ENDIAN
# define BDES_PLATFORMUTIL__IS_LITTLE_ENDIAN BDES_PLATFORMUTIL_IS_LITTLE_ENDIAN
#endif
#define BDES_PLATFORMUTIL__HTONL(X) BDES_PLATFORMUTIL_HTONL(X)
#define BDES_PLATFORMUTIL__HTONS(X) BDES_PLATFORMUTIL_HTONS(X)
#define BDES_PLATFORMUTIL__NTOHL(X) BDES_PLATFORMUTIL_NTOHL(X)
#define BDES_PLATFORMUTIL__NTOHS(X) BDES_PLATFORMUTIL_NTOHS(X)

// Note BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES is now never defined

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
