// bslh_siphashalgorithm.cpp                                          -*-C++-*-
#include <bslh_siphashalgorithm.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_types.h>
#include <bsls_platform.h>
#include <bsls_byteorder.h>

#include <algorithm>
#include <stddef.h>  // for 'size_t'
#include <string.h>

///Changes
///-------
// The third party code begins with the "siphash.h" header below, and continues
// until the Bloomberg LP copyright banner below.  Changes made to the original
// code include:
//
//: 1  Adding BloombergLP and bslh namespaces
//:
//: 2  Renaming 'siphash' to 'SipHashAlgorithm'
//:
//: 3  White space to meet BDE standards.
//:
//: 4  Added initializer list to handle class member initializers removed from
//:    the header
//:
//: 5  Added 'computeHash()' to handle explicit conversion removed from header.
//:
//: 6  Changed the constructor to accept a 'const char *'
//:
//: 7  Changed endianness check to use BDE-defined check
//:
//: 8  Removed unnamed namespace in favour of 'static'
//:
//: 9  Added function contracts
//:
//: 10 Added asserts to check passed pointers
//:
//: 11 Re-wrote unaligned reads to buffer data and then perform aligned reads
//:    to prevent runtime errors in opt builds on Sun machines.
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
// worldwide.  This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software.  If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
//-----------------------------------------------------------------------------


namespace BloombergLP {

namespace bslh {

typedef bsls::Types::Uint64 u64;
typedef unsigned int        u32;
typedef unsigned char       u8;

inline
static u64 rotl(u64 x, u64 b)
    // Return the bits of the specified 'x' rotated to the left by  the
    // specified 'b' number of bits.  Bits that are rotated off the end are
    // wrapped around to the beginning.
{
    return (x << b) | (x >> (64 - b));
}

inline
static void sipround(u64& v0, u64& v1, u64& v2, u64& v3)
    // Mix the specified 'v0', 'v1', 'v2', and 'v3' in a "Sip Round" as
    // described in the SipHash algorithm
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

inline
static u64 u8to64_le(const u8* p)
    // Return the 64-bit integer representation of the specified 'p' taking
    // into account endianness.  Undefined unless 'p' points to at least eight
    // bytes of initialized memory.
{
    BSLS_ASSERT(p);

#if defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64)
    return *reinterpret_cast<const u64 *>(p);  // Ignore alignment.
#else
    u64 ret;
    memcpy(&ret, p, sizeof(ret));
    return BSLS_BYTEORDER_LE_U64_TO_HOST(ret);
#endif
}


SipHashAlgorithm::SipHashAlgorithm(const char *seed)
: d_v0(0x736f6d6570736575ULL)
, d_v1(0x646f72616e646f6dULL)
, d_v2(0x6c7967656e657261ULL)
, d_v3(0x7465646279746573ULL)
, d_bufSize(0)
, d_totalLength(0)
{
    BSLS_ASSERT(seed);

    u64 k0 = u8to64_le(reinterpret_cast<const u8*>(&seed[0]));
    u64 k1 = u8to64_le(reinterpret_cast<const u8*>(&seed[k_SEED_LENGTH / 2]));

    d_v0 ^= k0;
    d_v1 ^= k1;
    d_v2 ^= k0;
    d_v3 ^= k1;
}

void
SipHashAlgorithm::operator()(const void *data, size_t numBytes)
{
    BSLS_ASSERT(data);

    u8 const* in = static_cast<const u8*>(data);

    d_totalLength += numBytes;
    if (d_bufSize + numBytes < 8)
    {
        std::copy(in, in + numBytes, d_buf + d_bufSize);
        d_bufSize += numBytes;
        return;                                                       // RETURN
    }
    if (d_bufSize > 0)
    {
        size_t t = 8 - d_bufSize;
        std::copy(in, in+t, d_buf + d_bufSize);
        u64 m = u8to64_le( d_buf );
        d_v3 ^= m;
        sipround(d_v0, d_v1, d_v2, d_v3);
        sipround(d_v0, d_v1, d_v2, d_v3);
        d_v0 ^= m;
        in += t;
        numBytes -= t;
    }
    d_bufSize = numBytes & 7;
    u8 const* const end = in + (numBytes - d_bufSize);
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

}  // close package namespace

}  // close enterprise namespace


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

