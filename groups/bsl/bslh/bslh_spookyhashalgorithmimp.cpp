// bslh_spookyhashalgorithmimp.cpp                                    -*-C++-*-
#include <bslh_spookyhashalgorithmimp.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///Changes
///-------
// The third party code begins with the "Spooky Hash" header below, and
// continues until the BloombergLP copyright notice.  Changes made to the
// original code include:
//
//: 1 Added 'BloombergLP' and 'bslh' namespaces
//:
//: 2 Renamed 'SpookyHash' to 'SpookyHashAlgorithmImp'
//:
//: 3 Changed formatting to match BDE conventions
//:
//: 4 Changed C-style casts to 'static_cast's to match BDE conventions
//:
//: 5 Made function names lower case (had to change 'Final' to 'finalize' and
//:   'Short' to 'shortHash' to avoid using a keyword)
//:
//: 6 Renamed 'end' varibles to 'endPtr' to avoid name conflict with renamed
//:   function member.
//:
//: 7 Included 'cstring' rather than 'memory'
//:
//: 8 Reordered variable declarations to allow benefit from early returns
//:
//: 9 Added '#ifdef' to prevent unaligned reads on Solaris
//
///Third Party Doc
///---------------
//
// Spooky Hash
// A 128-bit non cryptographic hash, for checksums and table lookup
//
// By Bob Jenkins.  Public domain.
//   Oct 31 2010: published framework, disclaimer ShortHash isn't right
//   Nov 7 2010: disabled ShortHash
//   Oct 31 2011: replace End, ShortMix, ShortEnd, enable ShortHash again
//   April 10 2012: buffer overflow on platforms without unaligned reads
//   July 12 2012: was passing out variables in final to in/out in short
//   July 30 2012: I reintroduced the buffer overflow
//   August 5 2012: SpookyV2: d = should be d += in short hash, and remove
//   extra mix from long hash

#include <bsls_platform.h>

#include <string.h> // for 'memcpy' and 'memset'

#if defined(BSLS_PLATFORM_CPU_X86_64) || defined(BSLS_PLATFORM_CPU_X86)
#define ALLOW_UNALIGNED_READS 1
#else
#define ALLOW_UNALIGNED_READS 0
#endif

namespace BloombergLP {

namespace bslh {

void SpookyHashAlgorithmImp::finalize(Uint64 *hash1, Uint64 *hash2)
{
    BSLS_ASSERT(hash1);
    BSLS_ASSERT(hash2);

    // Initialize the variables.
    if (m_length < k_BUFFER_SIZE)
    {
        *hash1 = m_state[0];
        *hash2 = m_state[1];
        shortHash( m_data, m_length, hash1, hash2);
        return;                                                       // RETURN
    }

    Uint64 *data = static_cast<Uint64 *>(m_data);
    Uint8 remainder = m_remainder;

    Uint64 h0 = m_state[0];
    Uint64 h1 = m_state[1];
    Uint64 h2 = m_state[2];
    Uint64 h3 = m_state[3];
    Uint64 h4 = m_state[4];
    Uint64 h5 = m_state[5];
    Uint64 h6 = m_state[6];
    Uint64 h7 = m_state[7];
    Uint64 h8 = m_state[8];
    Uint64 h9 = m_state[9];
    Uint64 h10 = m_state[10];
    Uint64 h11 = m_state[11];

    if (remainder >= k_BLOCK_SIZE)
    {
        // 'm_data' can contain two blocks; handle any whole first block.
        mix(data, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
        data      += k_NUM_VARS;
        remainder = static_cast<Uint8>(remainder - k_BLOCK_SIZE);
    }

    // Mix in the last partial block as well as the length mod 'k_BLOCK_SIZE'.
    memset(&(reinterpret_cast<Uint8 *>(data))[remainder], 0,
                                                   (k_BLOCK_SIZE - remainder));

    (reinterpret_cast<Uint8 *>(data))[k_BLOCK_SIZE - 1] = remainder;

    // Do some final mixing.
    end(data, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);

    *hash1 = h0;
    *hash2 = h1;
}

void SpookyHashAlgorithmImp::hash128(
    const void *message,
    size_t length,
    Uint64 *hash1,
    Uint64 *hash2)
{
    BSLS_ASSERT(message);
    BSLS_ASSERT(hash1);
    BSLS_ASSERT(hash2);

    if (length < k_BUFFER_SIZE)
    {
        shortHash(message, length, hash1, hash2);
        return;                                                       // RETURN
    }

    Uint64 h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11;
    Uint64 buf[k_NUM_VARS];
    Uint64 *endPtr;
    union
    {
        const Uint8 *p8;
        Uint64 *p64;
        size_t i;
    } u;
    size_t remainder;

    h0=h3=h6=h9  = *hash1;
    h1=h4=h7=h10 = *hash2;
    h2=h5=h8=h11 = sc_const;

    u.p8 = static_cast<const Uint8 *>(message);
    endPtr = u.p64 + (length/k_BLOCK_SIZE)*k_NUM_VARS;

    // Handle all whole 'k_BLOCK_SIZE' blocks of bytes.
    if (ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0))
    {
        while (u.p64 < endPtr)
        {
            mix(u.p64, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += k_NUM_VARS;
        }
    }
    else
    {
        while (u.p64 < endPtr)
        {
            memcpy(buf, u.p64, k_BLOCK_SIZE);
            mix(buf, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += k_NUM_VARS;
        }
    }

    // Handle the last partial block of k_BLOCK_SIZE bytes.
    remainder = (length - (reinterpret_cast<const Uint8 *>(endPtr) -
                           static_cast<const Uint8 *>(message)));
    memcpy(buf, endPtr, remainder);
    memset((reinterpret_cast<Uint8 *>(buf)) + remainder, 0,
                                                       k_BLOCK_SIZE-remainder);

    BSLS_ASSERT(remainder <= 255);
    (reinterpret_cast<Uint8 *>(buf))[k_BLOCK_SIZE-1] =
                                                 static_cast<Uint8>(remainder);

    // do some final mixing
    end(buf, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    *hash1 = h0;
    *hash2 = h1;
}

void SpookyHashAlgorithmImp::shortHash(
    const void *message,
    size_t length,
    Uint64 *hash1,
    Uint64 *hash2)
{
    BSLS_ASSERT(message);
    BSLS_ASSERT(hash1);
    BSLS_ASSERT(hash2);

    Uint64 buf[2*k_NUM_VARS];
    union
    {
        const Uint8 *p8;
        Uint32 *p32;
        Uint64 *p64;
        size_t i;
    } u;

    u.p8 = static_cast<const Uint8 *>(message);

    if (!ALLOW_UNALIGNED_READS && (u.i & 0x7))
    {
        memcpy(buf, message, length);
        u.p64 = buf;
    }

    size_t remainder = length%32;
    Uint64 a=*hash1;
    Uint64 b=*hash2;
    Uint64 c=sc_const;
    Uint64 d=sc_const;

    if (length > 15)
    {
        const Uint64 *endPtr = u.p64 + (length/32)*4;

        // Handle all complete sets of 32 bytes.
        for (; u.p64 < endPtr; u.p64 += 4)
        {
            c += u.p64[0];
            d += u.p64[1];
            shortMix(a,b,c,d);
            a += u.p64[2];
            b += u.p64[3];
        }

        // Handle the case of 16+ remaining bytes.
        if (remainder >= 16)
        {
            c += u.p64[0];
            d += u.p64[1];
            shortMix(a,b,c,d);
            u.p64 += 2;
            remainder -= 16;
        }
    }

    // Handle the last 0 to 15 bytes, and their length.
    d += (static_cast<Uint64>(length)) << 56;
    switch (remainder)
    // Note that this statement relies on fall-through behaviour.
    {
    case 15:
        d += (static_cast<Uint64>(u.p8[14])) << 48;
    case 14:
        d += (static_cast<Uint64>(u.p8[13])) << 40;
    case 13:
        d += (static_cast<Uint64>(u.p8[12])) << 32;
    case 12:
        d += u.p32[2];
        c += u.p64[0];
        break;
    case 11:
        d += (static_cast<Uint64>(u.p8[10])) << 16;
    case 10:
        d += (static_cast<Uint64>(u.p8[9])) << 8;
    case 9:
        d += static_cast<Uint64>(u.p8[8]);
    case 8:
        c += u.p64[0];
        break;
    case 7:
        c += (static_cast<Uint64>(u.p8[6])) << 48;
    case 6:
        c += (static_cast<Uint64>(u.p8[5])) << 40;
    case 5:
        c += (static_cast<Uint64>(u.p8[4])) << 32;
    case 4:
        c += u.p32[0];
        break;
    case 3:
        c += (static_cast<Uint64>(u.p8[2])) << 16;
    case 2:
        c += (static_cast<Uint64>(u.p8[1])) << 8;
    case 1:
        c += static_cast<Uint64>(u.p8[0]);
        break;
    case 0:
        c += sc_const;
        d += sc_const;
    }
    shortEnd(a,b,c,d);
    *hash1 = a;
    *hash2 = b;
}

void SpookyHashAlgorithmImp::update(const void *message, size_t length)
{
    BSLS_ASSERT(message);

    union
    {
        const Uint8 *p8;
        Uint64 *p64;
        size_t i;
    } u;

    // Store the message fragment for later use if it is too short.
    size_t newLength = length + m_remainder;
    if (newLength < k_BUFFER_SIZE)
    {
        memcpy(&(reinterpret_cast<Uint8 *>(m_data))[m_remainder], message,
                                                                       length);
        m_length = length + m_length;
        m_remainder = static_cast<Uint8>(newLength);
        return;                                                       // RETURN
    }

    // Declare and initialize the variables.
    Uint64 h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11;
    if (m_length < k_BUFFER_SIZE)
    {
        h0=h3=h6=h9  = m_state[0];
        h1=h4=h7=h10 = m_state[1];
        h2=h5=h8=h11 = sc_const;
    }
    else
    {
        h0 = m_state[0];
        h1 = m_state[1];
        h2 = m_state[2];
        h3 = m_state[3];
        h4 = m_state[4];
        h5 = m_state[5];
        h6 = m_state[6];
        h7 = m_state[7];
        h8 = m_state[8];
        h9 = m_state[9];
        h10 = m_state[10];
        h11 = m_state[11];
    }
    m_length = length + m_length;

    // If we have any message fragments stored for later use, handle them now.
    if (m_remainder)
    {
        BSLS_ASSERT(m_remainder < k_BUFFER_SIZE);

        Uint8 prefix = static_cast<Uint8>(k_BUFFER_SIZE - m_remainder);
        memcpy(&((reinterpret_cast<Uint8 *>(m_data))[m_remainder]), message,
                                                                       prefix);
        u.p64 = m_data;
        mix(u.p64, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
        mix(&u.p64[k_NUM_VARS], h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
        u.p8 = (static_cast<const Uint8 *>(message)) + prefix;
        length -= prefix;
    }
    else
    {
        u.p8 = static_cast<const Uint8 *>(message);
    }

    // Handle all whole blocks of 'k_BLOCK_SIZE' bytes.
    const Uint64 *endPtr = u.p64 + (length/k_BLOCK_SIZE)*k_NUM_VARS;
    Uint8 remainder = static_cast<Uint8>(length -
                               (reinterpret_cast<const Uint8 *>(endPtr)-u.p8));
    if (ALLOW_UNALIGNED_READS || (u.i & 0x7) == 0)
    {
        while (u.p64 < endPtr)
        {
            mix(u.p64, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += k_NUM_VARS;
        }
    }
    else
    {
        while (u.p64 < endPtr)
        {
            memcpy(m_data, u.p8, k_BLOCK_SIZE);
            mix(m_data, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += k_NUM_VARS;
        }
    }

    // Store the last few bytes for later use.
    m_remainder = remainder;
    memcpy(m_data, endPtr, remainder);

    // Save our state.
    m_state[0] = h0;
    m_state[1] = h1;
    m_state[2] = h2;
    m_state[3] = h3;
    m_state[4] = h4;
    m_state[5] = h5;
    m_state[6] = h6;
    m_state[7] = h7;
    m_state[8] = h8;
    m_state[9] = h9;
    m_state[10] = h10;
    m_state[11] = h11;
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

