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

                         // =========================
                         // struct ByteOrderUtil_Impl
                         // =========================

template <class T, Types::size_type WIDTH = sizeof(T)>
struct ByteOrderUtil_Impl;

template <class T>
struct ByteOrderUtil_Impl<T, 1> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static T swapBytes(T x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class T>
struct ByteOrderUtil_Impl<T, 2> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static T swapBytes(T x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class T>
struct ByteOrderUtil_Impl<T, 4> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static T swapBytes(T x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class T>
struct ByteOrderUtil_Impl<T, 8> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static T swapBytes(T x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

                          // ====================
                          // struct ByteOrderUtil
                          // ====================

struct ByteOrderUtil : public ByteOrderUtil_Impl<bool>,
                       public ByteOrderUtil_Impl<char>,
                       public ByteOrderUtil_Impl<unsigned char>,
                       public ByteOrderUtil_Impl<signed char>,
                       public ByteOrderUtil_Impl<wchar_t>,
                       public ByteOrderUtil_Impl<short>,
                       public ByteOrderUtil_Impl<unsigned short>,
                       public ByteOrderUtil_Impl<int>,
                       public ByteOrderUtil_Impl<unsigned int>,
                       public ByteOrderUtil_Impl<long>,
                       public ByteOrderUtil_Impl<unsigned long>,
                       public ByteOrderUtil_Impl<bsls::Types::Uint64>,
                       public ByteOrderUtil_Impl<bsls::Types::Int64> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    using ByteOrderUtil_Impl<bool               > ::swapBytes;
    using ByteOrderUtil_Impl<char               > ::swapBytes;
    using ByteOrderUtil_Impl<unsigned char      > ::swapBytes;
    using ByteOrderUtil_Impl<signed char        > ::swapBytes;
    using ByteOrderUtil_Impl<wchar_t            > ::swapBytes;
    using ByteOrderUtil_Impl<short              > ::swapBytes;
    using ByteOrderUtil_Impl<unsigned short     > ::swapBytes;
    using ByteOrderUtil_Impl<int                > ::swapBytes;
    using ByteOrderUtil_Impl<unsigned int       > ::swapBytes;
    using ByteOrderUtil_Impl<long               > ::swapBytes;
    using ByteOrderUtil_Impl<unsigned long      > ::swapBytes;
    using ByteOrderUtil_Impl<bsls::Types::Uint64> ::swapBytes;
    using ByteOrderUtil_Impl<bsls::Types::Int64 > ::swapBytes;

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

                         // -------------------------
                         // struct ByteOrderUtil_Impl
                         // -------------------------

// CLASS METHODS
template <class T>
inline
T ByteOrderUtil_Impl<T, 1>::swapBytes(T x)
{
    return x;
}

template <class T>
inline
T ByteOrderUtil_Impl<T, 2>::swapBytes(T x)
{
    // These macros all return a value of type 'T'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(T, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(T, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(T, x);
#endif
}

template <class T>
inline
T ByteOrderUtil_Impl<T, 4>::swapBytes(T x)
{
    // These macros all return a value of type 'T'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(T, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(T, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(T, x);
#endif
}

template <class T>
inline
T ByteOrderUtil_Impl<T, 8>::swapBytes(T x)
{
    // These macros all return a value of type 'T'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(T, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(T, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(T, x);
#endif
}

                            // -------------------
                            // class ByteOrderUtil
                            // -------------------

inline
unsigned short
ByteOrderUtil::swapBytes16(unsigned short x)
{
    // These macros all return a value of type 'unsigned short'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(unsigned short, x);
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
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(unsigned int, x);
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
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(bsls::Types::Uint64, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(bsls::Types::Uint64, &x);
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
