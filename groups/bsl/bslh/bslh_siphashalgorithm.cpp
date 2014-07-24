// bslh_siphashalgorithm.cpp                                          -*-C++-*-
#include <bslh_siphashalgorithm.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_types.h>

#include <algorithm>
#include <cstddef>

///Changes
///-------
// The third party code begins with the "siphash.h" header below, and continues
// untill the Bloomberg LP copyright banner below. Changes made to the original
// code include:
//
//: 1 Adding BloombergLP and bslh namespaces
//:
//: 2 Renaming 'siphash' to 'SipHashAlgorithm'
//:
//: 3 Whitespace to meet BDE standards.
//:
//: 4 Added initializer list to handle class member initializers removed from
//:   the header
//:
//: 5 Added 'computeHash' to handle explicit conversion removed from header.
//:
//: 6 Changed the constructor to accept a 'const char *'
//
///Third Party Doc
///---------------
//------------------------------- siphash.h -----------------------------------
//
// This software is in the public domain.  The only restriction on its use is
// that no one can remove it from the public domain by claiming ownership of
// it, including the original authors.
//
// There is no warranty of correctness on the software contained herein.  Use
// at your own risk.
//
// Derived from:
//
// SipHash reference C implementation
//
// Written in 2012 by Jean-Philippe Aumasson <jeanphilippe.aumasson@gmail.com>
// Daniel J. Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
//-----------------------------------------------------------------------------


namespace BloombergLP {

namespace bslh {

namespace
{

typedef bsls::Types::Uint64 u64;
typedef unsigned int        u32;
typedef unsigned char       u8;

inline
u64 rotl(u64 x, u64 b)
{
    return (x << b) | (x >> (64 - b));
}

inline
u64 u8to64_le(const u8* p)
{
#ifdef __LITTLE_ENDIAN__
    return *static_cast<u64 const*>(static_cast<void const*>(p));
#else
    return static_cast<u64>(p[7]) << 56 | static_cast<u64>(p[6]) << 48 |
           static_cast<u64>(p[5]) << 40 | static_cast<u64>(p[4]) << 32 |
           static_cast<u64>(p[3]) << 24 | static_cast<u64>(p[2]) << 16 |
           static_cast<u64>(p[1]) <<  8 | static_cast<u64>(p[0]);
#endif
}

inline
void sipround(u64& v0, u64& v1, u64& v2, u64& v3)
{
    v0 += v1;
    v1 = rotl(v1, 13);
    v1 ^= v0;
    v0 = rotl(v0, 32);
    v2 += v3;
    v3 = rotl(v3, 16);
    v3 ^= v2;
    v0 += v3;
    v3 = rotl(v3, 21);
    v3 ^= v0;
    v2 += v1;
    v1 = rotl(v1, 17);
    v1 ^= v2;
    v2 = rotl(v2, 32);
}

}  // unnamed


SipHashAlgorithm::SipHashAlgorithm(const char *seed) :
                                                   d_v0(0x736f6d6570736575ULL),
                                                   d_v1(0x646f72616e646f6dULL),
                                                   d_v2(0x6c7967656e657261ULL),
                                                   d_v3(0x7465646279746573ULL),
                                                   d_bufSize(0),
                                                   d_totalLength(0)
{
    const u64 *seedPtr = reinterpret_cast<const u64 *>(seed);
    d_v3 ^= seedPtr[1];
    d_v2 ^= seedPtr[0];
    d_v1 ^= seedPtr[1];
    d_v0 ^= seedPtr[0];
}

void
SipHashAlgorithm::operator()(void const* key, size_t inlen)
{
    u8 const* in = static_cast<const u8*>(key);
    d_totalLength += inlen;
    if (d_bufSize + inlen < 8)
    {
        std::copy(in, in+inlen, d_buf + d_bufSize);
        d_bufSize += inlen;
        return;                                                       // RETURN
    }
    if (d_bufSize > 0)
    {
        unsigned int t = 8 - d_bufSize;
        std::copy(in, in+t, d_buf + d_bufSize);
        u64 m = u8to64_le( d_buf );
        d_v3 ^= m;
        sipround(d_v0, d_v1, d_v2, d_v3);
        sipround(d_v0, d_v1, d_v2, d_v3);
        d_v0 ^= m;
        in += t;
        inlen -= t;
    }
    d_bufSize = inlen & 7;
    u8 const* const end = in + (inlen - d_bufSize);
    for ( ; in != end; in += 8 )
    {
        u64 m = u8to64_le( in );
        d_v3 ^= m;
        sipround(d_v0, d_v1, d_v2, d_v3);
        sipround(d_v0, d_v1, d_v2, d_v3);
        d_v0 ^= m;
    }
    std::copy(end, end + d_bufSize, d_buf);
}

SipHashAlgorithm::result_type SipHashAlgorithm::computeHash()
{
    result_type b = static_cast<u64>(d_totalLength) << 56;
    switch(d_bufSize)
    {
    case 7:
        b |= static_cast<u64>(d_buf[6]) << 48;
    case 6:
        b |= static_cast<u64>(d_buf[5]) << 40;
    case 5:
        b |= static_cast<u64>(d_buf[4]) << 32;
    case 4:
        b |= static_cast<u64>(d_buf[3]) << 24;
    case 3:
        b |= static_cast<u64>(d_buf[2]) << 16;
    case 2:
        b |= static_cast<u64>(d_buf[1]) << 8;
    case 1:
        b |= static_cast<u64>(d_buf[0]);
    case 0:
        break;
    }
    d_v3 ^= b;
    sipround(d_v0, d_v1, d_v2, d_v3);
    sipround(d_v0, d_v1, d_v2, d_v3);
    d_v0 ^= b;
    d_v2 ^= 0xff;
    sipround(d_v0, d_v1, d_v2, d_v3);
    sipround(d_v0, d_v1, d_v2, d_v3);
    sipround(d_v0, d_v1, d_v2, d_v3);
    sipround(d_v0, d_v1, d_v2, d_v3);
    b = d_v0 ^ d_v1 ^ d_v2  ^ d_v3;
    return b;
}

}  // close namespace bslh

}  // close namespace BloombergLP


// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------

