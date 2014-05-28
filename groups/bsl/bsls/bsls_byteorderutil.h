// bsls_byteorderutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDERUTIL
#define INCLUDED_BSLS_BYTEORDERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide byte-order manipulation macros.
//
//@CLASSES: bsls::ByteOrderUtil
//
//@SEE ALSO: bsls_byteorder, bsls_byteorderutil_impl
//
//@DESCRIPTION: This component provides a namespace, 'class'
// 'bsls::ByteOrderUtil', that contains a suite of static functions for
// reversing the byte order of integral types.  The functions
// 'swapByteOrder{16,32,64}' reverse the byte order of words of specifiedd
// widths (in bits), while the overloaded function 'swapBytes' will swap the
// bytes of any integral type passed to it, returning the same type it is
// passed.
//
///Usage
///-----
// In this example we demonstrate the use of different overloads of the
// 'swapBytes' function.
//
// First we typedef a shorthand to the namespace class:
//..
//  typedef bsls::ByteOrderUtil Util;
//..
// Then, we demonstrate reversing the bytes of an unsigned short:
//..
//  unsigned short us = 0x1234;
//  assert(0x3412 == Util::swapBytes(us));
//..
// Next, we do a signed short:
//..
//  short ss = 0x4321;
//  assert(0x2143 == Util::swapBytes(ss));
//..
// Then, we reverse an unsigned int:
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

#ifndef INCLUDED_BSLS_BYTEORDERUTIL_IMPL
#include <bsls_byteorderutil_impl.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM_CMP_HP

#ifndef INCLUDED_MACHINE_SYS_BUILTINS
#include <machine/sys/builtins.h>  // '_Asm_xchg'
#define INCLUDED_MACHINE_SYS_BUILTINS
#endif

#ifndef INCLUDED_MACHINE_SYS_INLINE
#include <machine/sys/inline.h>    // '_SZ_H', '_SZ_W', '_SZ_D'
#define INCLUDED_MACHINE_SYS_INLINE
#endif

#endif

#ifdef BSLS_PLATFORM_CMP_MSVC

#ifndef INCLUDED_STDLIB
#include <stdlib.h>        // '_byteswap_*'
#define INCLUDED_STDLIB
#endif

#endif

namespace BloombergLP {
namespace bsls {

                          // ====================
                          // struct ByteOrderUtil
                          // ====================

class ByteOrderUtil {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of integral types.

    // PRIVATE TYPES
    typedef ByteOrderUtil_Impl Impl;

  public:
    // PUBLIC CLASS METHODS

    // LEVEL 0 METHODS

    static unsigned short swapBytes16(unsigned short x);
        // Return the specified 'x' with byte order swapped.

    static unsigned int   swapBytes32(unsigned int   x);
        // Return the specified 'x' with byte order swapped.

    static Types::Uint64  swapBytes64(Types::Uint64  x);
        // Return the specified 'x' with byte order swapped.

    // LEVEL 1 METHODS

    static bool           swapBytes(bool           x);
    static char           swapBytes(char           x);
    static unsigned char  swapBytes(unsigned char  x);
    static signed char    swapBytes(signed char    x);
    static wchar_t        swapBytes(wchar_t        x);
    static short          swapBytes(short          x);
    static unsigned short swapBytes(unsigned short x);
    static int            swapBytes(int            x);
    static unsigned int   swapBytes(unsigned int   x);
    static long           swapBytes(long           x);
    static unsigned long  swapBytes(unsigned long  x);
    static Types::Uint64  swapBytes(Types::Uint64  x);
    static Types::Int64   swapBytes(Types::Int64   x);
        // If the specified 'x' is a multiple byte entity, return its value
        // with the byte order reversed, otherwise, return its value without
        // modification.
};

//=============================================================================
//                          INLINE FUNCTION DEFINITIONS
//=============================================================================

// LEVEL 0 METHODS

inline
unsigned short
ByteOrderUtil::swapBytes16(unsigned short x)
{
#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16)
    return Impl::custom_swap_16(x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16)
    return Impl::custom_swap_p16(&x);
#else
    return Impl::generic_swap_16(x);
#endif
}

inline
unsigned int
ByteOrderUtil::swapBytes32(unsigned int x)
{
#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32)
    return Impl::custom_swap_32(x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32)
    return Impl::custom_swap_p32(&x);
#else
    return Impl::generic_swap_32(x);
#endif
}

inline
bsls::Types::Uint64
ByteOrderUtil::swapBytes64(bsls::Types::Uint64 x)
{
#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64)
    return Impl::custom_swap_64(x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64)
    return Impl::custom_swap_p64(&x);
#else
    return Impl::generic_swap_64(x);
#endif
}

// LEVEL 1 METHODS

inline
bool ByteOrderUtil::swapBytes(bool x)
{
    return x;
}

inline
char ByteOrderUtil::swapBytes(char x)
{
    return x;
}

inline
unsigned char ByteOrderUtil::swapBytes(unsigned char x)
{
    return x;
}

inline
signed char ByteOrderUtil::swapBytes(signed char x)
{
    return x;
}

inline
wchar_t ByteOrderUtil::swapBytes(wchar_t x)
{
#if defined(BSLS_PLATFORM_CMP_MSVC) ||                                        \
    (defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CPU_32_BIT))
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(x));

    return static_cast<wchar_t>(swapBytes16(x));
#else
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(x));

    return static_cast<wchar_t>(swapBytes32(x));
#endif
}

inline
short ByteOrderUtil::swapBytes(short x)
{
    return static_cast<short>(swapBytes16(x));
}

inline
unsigned short ByteOrderUtil::swapBytes(unsigned short x)
{
    return swapBytes16(x);
}

inline
int ByteOrderUtil::swapBytes(int x)
{
    return static_cast<int>(swapBytes32(x));
}

inline
unsigned int ByteOrderUtil::swapBytes(unsigned int x)
{
    return swapBytes32(x);
}

inline
long ByteOrderUtil::swapBytes(long x)
{
#if defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_UNIX)
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));

    return static_cast<long>(swapBytes64(x));
#else
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(x));

    return static_cast<long>(swapBytes32(x));
#endif
}

inline
unsigned long ByteOrderUtil::swapBytes(unsigned long x)
{
#if defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_UNIX)
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));

    return swapBytes64(x);
#else
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(x));

    return swapBytes32(x);
#endif
}

inline
bsls::Types::Uint64 ByteOrderUtil::swapBytes(bsls::Types::Uint64 x)
{
    return swapBytes64(x);
}

inline
bsls::Types::Int64 ByteOrderUtil::swapBytes(bsls::Types::Int64 x)
{
    return static_cast<Types::Int64>(swapBytes64(x));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
