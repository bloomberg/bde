// bdlb_hashutil.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_hashutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_hashutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

// IMPLEMENTATION NOTES: See 'http://burtleburtle.net/bob/hash/evahash.html'.
// In particular this hash function has the NoFunnel property, defined in that
// reference as follows:
//..
//  Define 'h' to be a funneling hash if there is some subset 't' of the input
//  bits which can only affect bits 'u' in the internal state, and '|t| > |u|'
//  and 'v > |u|'.  'h' has a funnel of those 't' input bits into those 'u'
//  bits of the internal state.  If a hash has a funnel of 't' bits into 'u',
//  then 'u' of those 't' bits can cancel out the effects of the other
//  '|t|-|u|'.  The set of keys differing only in the input bits of the funnel
//  can produce no more than half that number of hash values.  (Those '2^|t|'
//  keys can produce no more than '2^|u|' out of '2^v' hash values.)
//..
// Differing in only a few bits is a common pattern in human and computer keys,
// so a funneling hash is seriously flawed.  In that reference, it is claimed
// that these hashes have no funnels, more specifically:
//..
// There is a funnel of 32 bits to 31 bits, with those 32 bits distributed
// across two blocks.  I backed up my computer, wrote a program that found
// this, then changed computers.  So I don't have the code and don't remember
// where the funnel was.  A funnel of 32 bits to 31 is awfully non-serious,
// though, so I let things be.
//..

namespace BloombergLP {

// CONSTANTS
const unsigned int k_ADDEND       = 1013904223;
const unsigned int k_MULTIPLICAND =    1664525;
const unsigned int k_MASK         = 4294967295U;

// STATIC HELPER FUNCTIONS
static inline
void mix1(unsigned int& a, unsigned int& b, unsigned int& c)
{
    a -= b;  a -= c;  a ^= (c >> 13);
    b -= c;  b -= a;  b ^= (a <<  8);
    c -= a;  c -= b;  c ^= (b >> 13);
    a -= b;  a -= c;  a ^= (c >> 12);
    b -= c;  b -= a;  b ^= (a << 16);
    c -= a;  c -= b;  c ^= (b >>  5);
    a -= b;  a -= c;  a ^= (c >>  3);
    b -= c;  b -= a;  b ^= (a << 10);
    c -= a;  c -= b;  c ^= (b >> 15);
}

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
static
unsigned int reverse_hash1(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    // Set up the internal state.

    const Ub1 *k   = reinterpret_cast<const Ub1 *>(data) + (length - 1);
    Ub4        len = length;
    Ub4        a   = 0x9e3779b9;  // the golden ratio; an arbitrary value
    Ub4        b   = a;
    Ub4        c   = 0;           // the previous hash value -- dummy

    // Handle most of the data.

    while (len >= 12) {
        a += k[ 0] + static_cast<Ub4>(k[ -1]<< 8)
                   + static_cast<Ub4>(k[ -2]<<16)
                   + static_cast<Ub4>(k[ -3]<<24);

        b += k[-4] + static_cast<Ub4>(k[ -5]<< 8)
                   + static_cast<Ub4>(k[ -6]<<16)
                   + static_cast<Ub4>(k[ -7]<<24);

        c += k[-8] + static_cast<Ub4>(k[ -9]<< 8)
                   + static_cast<Ub4>(k[-10]<<16)
                   + static_cast<Ub4>(k[-11]<<24);
        mix1(a, b ,c);
        k -= 12;  len -= 12;
    }

    // Handle the last 11 bytes.

    c += length;

    switch (len) {         // All of the case statements FALL THROUGH.
      case 11: c += static_cast<Ub4>(k[-10] << 24);
      case 10: c += static_cast<Ub4>(k[ -9] << 16);
      case  9: c += static_cast<Ub4>(k[ -8] <<  8);
                           // The first byte of 'c' is reserved for the length.
      case  8: b += static_cast<Ub4>(k[ -7] << 24);
      case  7: b += static_cast<Ub4>(k[ -6] << 16);
      case  6: b += static_cast<Ub4>(k[ -5] <<  8);
      case  5: b += k[-4];
      case  4: a += static_cast<Ub4>(k[ -3] << 24);
      case  3: a += static_cast<Ub4>(k[ -2] << 16);
      case  2: a += static_cast<Ub4>(k[ -1] <<  8);
      case  1: a += k[0];
                           // case 0: nothing left to add
      default: break;
    }

    // Mix and return the result.

    mix1(a, b, c);

    return c;
}

static
unsigned int reverse_hash2(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    const Ub1 *k    = reinterpret_cast<const Ub1 *>(data);
    Ub4        hash = 0;

    for (int i = length; i > 0; /* noop */) {
        hash += k[--i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
#endif

namespace bdlb {
                            // ---------------
                            // struct HashUtil
                            // ---------------

// CLASS METHODS
unsigned int HashUtil::hash1(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    // Set up the internal state.

    const Ub1 *k   = reinterpret_cast<const Ub1 *>(data);
    Ub4        len = length;
    Ub4        a   = 0x9e3779b9;  // the golden ratio; an arbitrary value
    Ub4        b   = a;
    Ub4        c   = 0;           // the previous hash value -- dummy

    // Handle most of the data.

    while (len >= 12) {
        a += k[0] + static_cast<Ub4>(k[ 1]<< 8)
                  + static_cast<Ub4>(k[ 2]<<16)
                  + static_cast<Ub4>(k[ 3]<<24);

        b += k[4] + static_cast<Ub4>(k[ 5]<< 8)
                  + static_cast<Ub4>(k[ 6]<<16)
                  + static_cast<Ub4>(k[ 7]<<24);

        c += k[8] + static_cast<Ub4>(k[ 9]<< 8)
                  + static_cast<Ub4>(k[10]<<16)
                  + static_cast<Ub4>(k[11]<<24);
        mix1(a, b, c);
        k += 12;  len -= 12;
    }

    // Handle the last 11 bytes.

    c += length;

    switch (len) {         // All the case statements FALL THROUGH.
      case 11: c += static_cast<Ub4>(k[10] << 24);
      case 10: c += static_cast<Ub4>(k[ 9] << 16);
      case  9: c += static_cast<Ub4>(k[ 8] <<  8);
                           // The first byte of 'c' is reserved for the length.
      case  8: b += static_cast<Ub4>(k[ 7] << 24);
      case  7: b += static_cast<Ub4>(k[ 6] << 16);
      case  6: b += static_cast<Ub4>(k[ 5] <<  8);
      case  5: b += k[4];
      case  4: a += static_cast<Ub4>(k[ 3] << 24);
      case  3: a += static_cast<Ub4>(k[ 2] << 16);
      case  2: a += static_cast<Ub4>(k[ 1] <<  8);
      case  1: a += k[0];
                           // case 0: nothing left to add
      default: break;
    }

    // Mix and return the result.

    mix1(a, b, c);

    return c;
}

unsigned int HashUtil::hash2(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    const Ub1 *k    = reinterpret_cast<const Ub1 *>(data);
    Ub4        hash = 0;

    for (int i = 0; i < length; ++i) {
        hash += k[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
}  // close package namespace

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
  #define HASH1(KEY)  hash1((const char *)&KEY, sizeof KEY)
  #define HASH2(KEY)  hash2((const char *)&KEY, sizeof KEY)
#else
  #define HASH1(KEY)  reverse_hash1((const char *)&KEY, sizeof KEY)
  #define HASH2(KEY)  reverse_hash2((const char *)&KEY, sizeof KEY)
#endif

namespace bdlb {

unsigned int HashUtil::hash1(char key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(signed char key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(unsigned char key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(short key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(unsigned short key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(int key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(unsigned int key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(long key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(unsigned long key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(bsls::Types::Int64 key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(bsls::Types::Uint64 key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(float key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(double key)
{
    return HASH1(key);
}
unsigned int HashUtil::hash1(const void *key)
{
    return HASH1(key);
}

unsigned int HashUtil::hash2(char key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(signed char key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(unsigned char key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(short key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(unsigned short key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(int key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(unsigned int key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(long key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(unsigned long key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(bsls::Types::Int64 key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(bsls::Types::Uint64 key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(float key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(double key)
{
    return HASH2(key);
}
unsigned int HashUtil::hash2(const void *key)
{
    return HASH2(key);
}
}  // close package namespace

#undef HASH1
#undef HASH2

namespace bdlb {

unsigned int HashUtil::hash0(const char *string, int modulus)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 < modulus);

    unsigned int r = 0;

    if (4 == sizeof(int)) {
        while (*string) {
            r ^= *string++;
            r  = r * k_MULTIPLICAND + k_ADDEND;
        }
    }
    else {
        while (*string) {
            r ^= *string++;
            r  = (r * k_MULTIPLICAND + k_ADDEND) & k_MASK;
        }
    }

    return r % static_cast<unsigned int>(modulus);
}

unsigned int HashUtil::hash0(const char *string,
                             int         stringLength,
                             int         modulus)
{
    BSLS_ASSERT(0 <= stringLength);
    BSLS_ASSERT(string || 0 == stringLength);
    BSLS_ASSERT(0 < modulus);

    const char   *end = string + stringLength;
    unsigned int  r   = 0;

    if (4 == sizeof(int)) {
        while (string != end) {
            r ^= *string++;
            r  = r * k_MULTIPLICAND + k_ADDEND;
        }
    }
    else {
        while (string != end) {
            r ^= *string++;
            r  = (r * k_MULTIPLICAND + k_ADDEND) & k_MASK;
        }
    }

    return r % static_cast<unsigned int>(modulus);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
