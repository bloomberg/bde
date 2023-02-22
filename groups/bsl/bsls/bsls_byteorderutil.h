// bsls_byteorderutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDERUTIL
#define INCLUDED_BSLS_BYTEORDERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide byte-order swapping functions.
//
//@CLASSES:
//   bsls::ByteOrderUtil: namespace for byte-order swapping functions
//
//@SEE_ALSO: bsls_byteorder
//
//@DESCRIPTION: This component provides a utility 'class',
// 'bsls::ByteOrderUtil', that contains a suite of static functions for
// reversing the byte order of integral types.  The functions
// 'swapByteOrder{16,32,64}' reverse the byte order of words having the
// indicated widths (in bits), while the overloaded function 'swapBytes' will
// swap the bytes of any integral type passed to it, returning the same type it
// is passed.
//
///Usage
///-----
// In this example we demonstrate the use of different overloads of the
// 'swapBytes' function.
//
// First we 'typedef' a shorthand to the namespace 'class':
//..
//  typedef bsls::ByteOrderUtil Util;
//..
// Then, we demonstrate reversing the bytes of an 'unsigned short':
//..
//  unsigned short us = 0x1234;
//  assert(0x3412 == Util::swapBytes(us));
//..
// Next, we do a signed 'short:
//..
//  short ss = 0x4321;
//  assert(0x2143 == Util::swapBytes(ss));
//..
// Then, we reverse an 'unsigned int':
//..
//  unsigned int ui = 0x01020304;
//  assert(0x04030201 == Util::swapBytes(ui));
//..
// Next, we reverse the bytes of a 32-bit signed integer:
//..
//  int si = 0x11223344;
//  assert(0x44332211 == Util::swapBytes(si));
//..
// Now, we perform the transform on a 64-bit unsigned:
//..
//  bsls::Types::Uint64 ui64 = 0x0102030405060708ULL;
//  assert(0x0807060504030201ULL == Util::swapBytes(ui64));
//..
// Finally, we do a 64-bit signed integer:
//..
//  bsls::Types::Int64 i64 = 0x0a0b0c0d0e0f0102LL;
//  assert(0x02010f0e0d0c0b0aLL == Util::swapBytes(i64));
//..

#include <bsls_byteorderutil_impl.h>
#include <bsls_platform.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bsls {

                          // ====================
                          // struct ByteOrderUtil
                          // ====================

struct ByteOrderUtil {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static bool           swapBytes(bool           x);
    static char           swapBytes(char           x);
    static unsigned char  swapBytes(unsigned char  x);
    static signed char    swapBytes(signed char    x);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    static char8_t        swapBytes(char8_t        x);
#endif
    static wchar_t        swapBytes(wchar_t        x);
    static short          swapBytes(short          x);
    static unsigned short swapBytes(unsigned short x);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    static char16_t       swapBytes(char16_t       x);
    static char32_t       swapBytes(char32_t       x);
#endif
    static int            swapBytes(int            x);
    static unsigned int   swapBytes(unsigned int   x);
    static long           swapBytes(long           x);
    static unsigned long  swapBytes(unsigned long  x);
    static Types::Uint64  swapBytes(Types::Uint64  x);
    static Types::Int64   swapBytes(Types::Int64   x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static unsigned short swapBytes16(unsigned short x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static unsigned int   swapBytes32(unsigned int   x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static Types::Uint64  swapBytes64(Types::Uint64  x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct ByteOrderUtil
                            // --------------------

// CLASS METHODS
inline
bool
ByteOrderUtil::swapBytes(bool           x)
{
    return x;
}

inline
char
ByteOrderUtil::swapBytes(char           x)
{
    return x;
}

inline
unsigned char
ByteOrderUtil::swapBytes(unsigned char  x)
{
    return x;
}

inline
signed char
ByteOrderUtil::swapBytes(signed char    x)
{
    return x;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
inline
char8_t
ByteOrderUtil::swapBytes(char8_t         x)
{
    return x;
}
#endif

inline
wchar_t
ByteOrderUtil::swapBytes(wchar_t        x)
{
    // Size of 'wchar_t' varies depending on platform and compiler switches.
    // We could not find any compiler-defined macros that would reliably
    // indicate the size, so use 'ByteOrderUtil_Impl' to adjust automatically.

    return ByteOrderUtil_Impl<wchar_t>::swapBytes(x);
}

inline
short
ByteOrderUtil::swapBytes(short          x)
{
    // These macros all return a value of type 'short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( short, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(short, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(short, x);
#endif
}

inline
unsigned short
ByteOrderUtil::swapBytes(unsigned short x)
{
    // These macros all return a value of type 'unsigned short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( unsigned short, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(unsigned short, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(unsigned short, x);
#endif
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
char16_t
ByteOrderUtil::swapBytes(char16_t x)
{
    // These macros all return a value of type 'unsigned short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( char16_t, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(char16_t, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(char16_t, x);
#endif
}

inline
char32_t
ByteOrderUtil::swapBytes(char32_t       x)
{
    // These macros all return a value of type 'int'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( char32_t, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(char32_t, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(char32_t, x);
#endif
}
#endif

inline
int
ByteOrderUtil::swapBytes(int            x)
{
    // These macros all return a value of type 'int'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( int, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(int, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(int, x);
#endif
}

inline
unsigned int
ByteOrderUtil::swapBytes(unsigned int   x)
{
    // These macros all return a value of type 'unsigned int'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( unsigned int, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(unsigned int, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(unsigned int, x);
#endif
}

inline
long
ByteOrderUtil::swapBytes(long           x)
{
    // Size of 'long' varies depending on platform and compiler switches.  We
    // could not find any compiler-defined macros that would reliably indicate
    // the size, so use 'ByteOrderUtil_Impl' to adjust automatically.

    return ByteOrderUtil_Impl<long>::swapBytes(x);
}

inline
unsigned long
ByteOrderUtil::swapBytes(unsigned long  x)
{
    // Size of 'unsigned long' varies depending on platform and compiler
    // switches.  We could not find any compiler-defined macros that would
    // reliably indicate the size, so use 'ByteOrderUtil_Impl' to adjust
    // automatically.

    return ByteOrderUtil_Impl<unsigned long>::swapBytes(x);
}

inline
Types::Uint64
ByteOrderUtil::swapBytes(Types::Uint64  x)
{
    // These macros all return a value of type 'bsls::Types::Uint64'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64( bsls::Types::Uint64, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
# endif

    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(bsls::Types::Uint64, &x);

# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Uint64, x);
#endif
}

inline
Types::Int64
ByteOrderUtil::swapBytes(Types::Int64   x)
{
    // These macros all return a value of type 'bsls::Types::Int64'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64( bsls::Types::Int64, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
# endif

    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(bsls::Types::Int64, &x);

# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Int64, x);
#endif
}

inline
unsigned short
ByteOrderUtil::swapBytes16(unsigned short x)
{
    // These macros all return a value of type 'unsigned short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( unsigned short, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(unsigned short, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(unsigned short, x);
#endif
}

inline
unsigned int
ByteOrderUtil::swapBytes32(unsigned int x)
{
    // These macros all return a value of type 'unsigned int'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( unsigned int, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(unsigned int, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(unsigned int, x);
#endif
}

inline
bsls::Types::Uint64
ByteOrderUtil::swapBytes64(bsls::Types::Uint64 x)
{
    // These macros all return a value of type 'bsls::Types::Uint64'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64( bsls::Types::Uint64, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
# endif

    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(bsls::Types::Uint64, &x);

# ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   pragma GCC diagnostic pop
# endif
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(bsls::Types::Uint64, x);
#endif
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
