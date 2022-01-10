// bslh_wyhashalgorithm.cpp                                           -*-C++-*-
#include <bslh_wyhashalgorithm.h>

#include <bsls_byteorderutil.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

//includes
#include <stdint.h>
#include <string.h>
#if defined(_MSC_VER) && defined(_M_X64)
  #include <intrin.h>
  #pragma intrinsic(_umul128)
#endif

using namespace BloombergLP;

// This was downloaded from
// https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
// which had been updated on September 14, 2021, last commit 166f352

// Many cosmetic changes have been made.  There were also many functions not
// relevant to the hash function which have been removed.

// This is free and unencumbered software released into the public domain under
// The Unlicense (http://unlicense.org/)
// main repo: https://github.com/wangyi-fudan/wyhash
// author: Wang Yi <godspeed_china@yeah.net>
// contributors: Reini Urban, Dietrich Epp, Joshua Haberman, Tommy Ettinger,
// Daniel Lemire, Otmar Ertl, cocowalla, leo-yuriev, Diego Barrios Romero,
// paulie-g, dumblob, Yann Collet, ivte-ms, hyb, James Z.M. Gao,
// easyaspi314 (Devin), TheOneric

// quick example:
// string s="fjsakfdsjkf";
// uint64_t hash=wyhash(s.c_str(), s.size(), 0, _wyp);

#define wyhash_final_version_3

//protections that produce different results:
//: 0 normal valid behavior
//:
//: 1 extra protection against entropy loss (probability=2^-63), aka.  "blind
//:   multiplication"
#undef  U_WYMUM_XOR
#define U_WYMUM_XOR 0

//: 0 normal, real version of 64x64 -> 128 multiply, slow on 32 bit systems
//:
//: 1 not real multiply, faster on 32 bit systems but produces different
//:   results
#undef  U_WYMUM_PSEUDO_MULTIPLY
#define U_WYMUM_PSEUDO_MULTIPLY 0

//likely and unlikely macros
#undef  U_LIKELY
#define U_LIKELY(x)        BSLS_PERFORMANCEHINT_PREDICT_LIKELY(x)
#undef  U_UNLIKELY
#define U_UNLIKELY(x)      BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(x)
#undef  U_UNLIKELY_HINT
#define U_UNLIKELY_HINT    BSLS_PERFORMANCEHINT_UNLIKELY_HINT

static inline
uint64_t _wyrot(uint64_t x)
{
    return (x >> 32) | (x << 32);
}

static inline
void _wymum(uint64_t *A, uint64_t *B)
    // 128bit multiply function -- 64x64 -> 128,
    // Result: *A is low 64 bits, *B is high 64 bits
{
#if U_WYMUM_PSEUDO_MULTIPLY
    const uint64_t hh = (*A >> 32) * (*B >> 32);
    const uint64_t hl = (*A >> 32) * (uint32_t) *B;
    const uint64_t lh = (uint32_t)*A * (*B >> 32);
    const uint64_t ll = (uint64_t) (uint32_t) *A * (uint32_t) *B;

# if U_WYMUM_XOR
    // pseudo munge (not real multiply) -> xor

    *A ^= _wyrot(hl) ^ hh;
    *B ^= _wyrot(lh) ^ ll;
# else
    // pseudo munge (not real multiply)

    *A = _wyrot(hl) ^ hh;
    *B = _wyrot(lh) ^ ll;
# endif
#elif defined(__SIZEOF_INT128__)
    __uint128_t r = *A; r *= *B;

# if U_WYMUM_XOR
    // multiply -> xor

    *A ^= (uint64_t) r;
    *B ^= (uint64_t) (r >> 64);
# else
    // multiply

    *A = (uint64_t) r;
    *B = (uint64_t) (r >> 64);
# endif
#elif defined(_MSC_VER) && defined(_M_X64)
# if U_WYMUM_XOR
    // multiply -> xor

    uint64_t  a,  b;
    a = _umul128(*A, *B, &b);
    *A ^= a;
    *B ^= b;
# else
    // multiply

    *A = _umul128(*A, *B, B);
# endif
#else
    const uint64_t ha  = *A >> 32,      hb = *B >> 32;
    const uint64_t la  = (uint32_t) *A, lb = (uint32_t) *B;

    const uint64_t rh  = ha * hb, rl  = la * lb;
    const uint64_t rm0 = ha * lb, rm1 = hb * la;
    const uint64_t t   = rl + (rm0 << 32);

    const uint64_t lo  = t  + (rm1 << 32);
    const uint64_t hi  = rh + (rm0 >> 32) + (rm1 >> 32) + (t < rl) + (lo < t);

# if U_WYMUM_XOR
    // multiply -> xor

    *A ^= lo;
    *B ^= hi;
# else
    // multiply

    *A = lo;
    *B = hi;
# endif
#endif
}

//multiply and xor mix function, aka MUM
static inline
uint64_t _wymix(uint64_t A, uint64_t B)
{
    _wymum(&A,&B);
    return A^B;
}

//read functions
#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

static inline
uint64_t _wyr8(const uint8_t *p)
{
    uint64_t v;
    memcpy(&v, p, 8);
    return v;
}

static inline
uint64_t _wyr4(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
    return v;
}

#else

static inline
uint64_t _wyr8(const uint8_t *p)
{
    bsls::Types::Uint64 v;
    memcpy(&v, p, 8);

    return bsls::ByteOrderUtil::swapBytes64(v);
}

static inline
uint64_t _wyr4(const uint8_t *p)
{
    unsigned v;
    memcpy(&v, p, 4);

    return bsls::ByteOrderUtil::swapBytes32(v);
}

#endif

static inline
uint64_t _wyr3(const uint8_t *p, size_t k)
    // Read a mix of the 'k' bytes beginning at 'p', where 'k' is in the range
    // '[ 1 .. 3 ]'.
{
    BSLS_ASSERT_SAFE(1 <= k && k <= 3);

    return (((uint64_t) p[0]) << 16) | (((uint64_t) p[k >> 1]) << 8) |
                                                                      p[k - 1];
}

//wyhash main function
static inline
uint64_t wyhash(const uint8_t  *p,
                size_t          len,
                uint64_t        seed)
{
    // These values were copied directly from the original source.

    static const uint64_t secret[] = { 0xa0761d6478bd642full,
                                       0xe7037ed1a0b428dbull,
                                       0x8ebc6af09c88c6e3ull,
                                       0x589965cc75374cc3ull };

    seed ^= secret[0];
    uint64_t a, b;

    if (U_LIKELY(len <= 16)) {
        if (U_LIKELY(len >= 4)) {
            const size_t subLen = (len >> 3) << 2;

            a = (_wyr4(p)           << 32) | _wyr4(p           + subLen);
            b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - subLen);
        }
        else if (U_LIKELY(len > 0)) {
            a = _wyr3(p, len);
            b = 0;
        }
        else {
            U_UNLIKELY_HINT;

            a = b = 0;
        }
    }
    else {
        U_UNLIKELY_HINT;

        const uint8_t *end = p + len;

        if (U_UNLIKELY(end - p > 48)) {
            U_UNLIKELY_HINT;

            uint64_t see1 = seed, see2 = seed;

            do {
                seed = _wymix(_wyr8(p)      ^ secret[1], _wyr8(p +  8) ^ seed);
                see1 = _wymix(_wyr8(p + 16) ^ secret[2], _wyr8(p + 24) ^ see1);
                see2 = _wymix(_wyr8(p + 32) ^ secret[3], _wyr8(p + 40) ^ see2);
                p += 48;
            } while (U_LIKELY(end - p > 48));

            seed ^= see1 ^ see2;
        }

        while (U_UNLIKELY(end - p > 16)) {
            U_UNLIKELY_HINT;

            seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
            p += 16;
        }

        a = _wyr8(end - 16);
        b = _wyr8(end -  8);
    }

    return _wymix(secret[1] ^ len, _wymix(a ^ secret[1], b ^ seed));
}

namespace BloombergLP {
namespace bslh {

// MANIPULATORS
void WyHashAlgorithm::operator()(const void *data, size_t numBytes)
{
    BSLS_ASSERT(0 != data || 0 == numBytes);

    d_data ^= wyhash(static_cast<const uint8_t *>(data), numBytes, d_data);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
