// bsls_byteorderutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDERUTIL
#define INCLUDED_BSLS_BYTEORDERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide byte-order swapping functions.
//
//@CLASSES:
//   bsls::ByteOrderUtil: namespace for byte-order swapping functions
//
//@SEE ALSO: bsls_byteorder
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

#ifndef INCLUDED_BSLS_BYTEORDERUTIL_IMPL
#include <bsls_byteorderutil_impl.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bsls {

                          // ===================
                          // class ByteOrderUtil
                          // ===================

struct ByteOrderUtil {
    // This utility struct provides a namespace for functions used for
    // reversing the byte order of values having integral type.

    // CLASS METHODS
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

                      // ===============================
                      // struct ByteOrderUtil_Dispatcher
                      // ===============================

template <size_t WIDTH>
class ByteOrderUtil_Dispatcher {
    // This class provides a set of namespaces for type- and size-specific swap
    // dispatch functions that call the size-appropriate 'ByteOrderUtil_Impl'
    // function for any type.

    // PRIVATE TYPES
    typedef ByteOrderUtil_Impl Impl;

  public:
    template <class T>
    static T swapBytes(T x);
};

// ============================================================================
//                                  LOCAL MACROS
// ============================================================================

                  // --------------------------------------------
                  // macro BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT
                  // --------------------------------------------

// We don't have access to 'BSLMF_ASSERT' here in 'bsls' -- do a crude
// compile-time assert for use in 'bsls_byteorderutil'.  This macro will
// deliberately cause a compilation error if 'expr' evaluates to 'false'.
// 'expr' must be a compile-time expression.  Note that this macro can only be
// called in a code body.  Also note that this macro is not to be used outside
// this file.

#define BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(expr)                          \
        { enum { k_NOT_INFINITY = 1 / static_cast<int>(expr) };               \
        (void) k_NOT_INFINITY; }

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // -------------------
                              // class ByteOrderUtil
                              // -------------------

// CLASS METHODS
inline
bool ByteOrderUtil::swapBytes(bool x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
char ByteOrderUtil::swapBytes(char x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
unsigned char ByteOrderUtil::swapBytes(unsigned char x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
signed char ByteOrderUtil::swapBytes(signed char x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
wchar_t ByteOrderUtil::swapBytes(wchar_t x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
short ByteOrderUtil::swapBytes(short x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
unsigned short ByteOrderUtil::swapBytes(unsigned short x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
int ByteOrderUtil::swapBytes(int x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
unsigned int ByteOrderUtil::swapBytes(unsigned int x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
long ByteOrderUtil::swapBytes(long x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
unsigned long ByteOrderUtil::swapBytes(unsigned long x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
bsls::Types::Uint64 ByteOrderUtil::swapBytes(bsls::Types::Uint64 x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
bsls::Types::Int64 ByteOrderUtil::swapBytes(bsls::Types::Int64 x)
{
    return ByteOrderUtil_Dispatcher<sizeof(x)>::swapBytes(x);
}

inline
unsigned short
ByteOrderUtil::swapBytes16(unsigned short x)
{
    return ByteOrderUtil_Dispatcher<2>::swapBytes(x);
}

inline
unsigned int
ByteOrderUtil::swapBytes32(unsigned int x)
{
    return ByteOrderUtil_Dispatcher<4>::swapBytes(x);
}

inline
bsls::Types::Uint64
ByteOrderUtil::swapBytes64(bsls::Types::Uint64 x)
{
    return ByteOrderUtil_Dispatcher<8>::swapBytes(x);
}

                      // -------------------------------
                      // struct ByteOrderUtil_Dispatcher
                      // -------------------------------

template <size_t WIDTH>
template <class T>
T ByteOrderUtil_Dispatcher<WIDTH>::swapBytes(T x)
{
    BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(false);
}

template <>
template <class T>
T ByteOrderUtil_Dispatcher<1>::swapBytes(T x)
{
    BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(1 == sizeof(x));

    return x;
}

template <>
template <class T>
T ByteOrderUtil_Dispatcher<2>::swapBytes(T x)
{
    BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(2 == sizeof(x));

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16)
    return static_cast<T>(Impl::customSwap16(x));
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16)
    return static_cast<T>(Impl::customSwapP16(&x));
#else
    return static_cast<T>(Impl::genericSwap16(x));
#endif
}

template <>
template <class T>
T ByteOrderUtil_Dispatcher<4>::swapBytes(T x)
{
    BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(4 == sizeof(x));

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32)
    return static_cast<T>(Impl::customSwap32(x));
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32)
    return static_cast<T>(Impl::customSwapP32(&x));
#else
    return static_cast<T>(Impl::genericSwap32(x));
#endif
}

template <>
template <class T>
T ByteOrderUtil_Dispatcher<8>::swapBytes(T x)
{
    BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT(8 == sizeof(x));

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64)
    return static_cast<T>(Impl::customSwap64(x));
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64)
    return static_cast<T>(Impl::customSwapP64(&x));
#else
    return static_cast<T>(Impl::genericSwap64(x));
#endif
}

}  // close package namespace
}  // close enterprise namespace

#undef BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT    // This macro is not for use
                                                 // outside this file.

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
