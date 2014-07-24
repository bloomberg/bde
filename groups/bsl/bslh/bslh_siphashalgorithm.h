// bslh_siphashalgorithm.h                                            -*-C++-*-
#ifndef INCLUDED_BSLH_SIPHASHALGORITHM
#define INCLUDED_BSLH_SIPHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the SipHash algorithm.
//
//@CLASSES:
// bslh::SipHashAlgorithm: SipHash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::SipHashAlgorithm' implements the SipHash algorithm.
// SipHash is an algorithm designed for speed and security.  A primary use case
// for this algorithm is to provide an extra line of defense in hash tables
// (such as the underlying implementation of unordered map) against malicious
// input that could cause denial of service attacks.  It is based on one of the
// finalists for the SHA-3 cryptographic hash standard.  Full details of the
// hash function can be found here: https://131002.net/siphash/siphash.pdf
// This particular implementation has been derived from Howard Hinnant's work
// here: https://github.com/HowardHinnant/hash_append/blob/master/siphash.h and
// as much of the original code as possible, including comment headers, has
// been preserved.
//
///Security Gaurentees
///-------------------
// SipHash is NOT a cryptographically secure hash. In the paper linked above,
// the creators of this hash function describe it as "Cryptographically
// Strong", but explicity avoid calling it cryptographically secure. In order
// to be cryptographically secure, and algorithm must, among other things,
// provide "collision resistance".  "Collision resistance" means that it should
// be difficult to find two different messages m1 and m2 such that hash(m1) =
// hash(m2).  Because of the limited sized output (only 2^64 possibilities) and
// the fast execution time of the algorithm, it is feasible to find collisions
// by brute force, making the algorithm not cryptographically secure.
//
// SipHash IS, however, a cryptographically strong PRF (pseudorandom function).
// This means that, assuming a properly random seed is given, the output of
// this algorithm will be indistinguishable from a uniform random distribution.
//
///Denial of Service Protection
///----------------------------
// The inability to predict the output of the hash from the input without
// exhaustive search means that this algorithm can help mitigate denial of
// service attacks on a hash table.  Denial of service attacks occur when an
// attacker deliberately degrades the performace of the hash table by inserting
// data that will collide to the same bucket, causing an O(1) lookup to become
// a O(n) linear search.  This protection is only effective if the seed
// provided is random and hidden from the attacker.
//
///Speed
///-----
// This algorithm is designed to be fast, compared to other algorithms making
// similar gaurentees. It is still slower than other commonly accepted and used
// hashes such as SpookyHash. This hash should only be used when protection
// from malicious input is required. Otherwise, 'bslh::DefaultHashAlgorithm'
// should be used to obtain a faster, generally applicable, hashing algorithm.
//
///Endianness
///----------
// This hash is endian-specific. The algorithm will run on big and little
// endian machines and the above gaurentees apply on both architectures,
// however, the hashes produced will be different. Be aware that this means
// storing hashes in memory or transmitting them across the network is not
// reccomended.
//
///Changes
///-------
// The third party code begins with the "siphash.h" header below, and continues
// untill the TYPE TRAITS banner below. Changes made to the original code
// include:
//
//: 1 Adding BloombergLP and bslh namespaces
//:
//: 2 Renaming 'siphash' to 'SipHashAlgorithm'
//:
//: 3 Whitespace changes for formatting
//:
//: 4 Added comments
//:
//: 5 Removed C++11 features including class member initializer, 'noexcept',
//:   'std::uint64_t', elxplicit conversion operator, and an '= default'
//:   constructor.
//:
//: 6 Added typedef to replace removed 'std::uint64_t'
//:
//: 7 Added 'computeHash' to replace the removed explicit conversion
//:
//: 8 Added 'k_SEED_LENGTH' and changed the constructor to accept a 'const char
//:   *'
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif




namespace BloombergLP {

namespace bslh {


                          // ============================
                          // class bslh::SipHashAlgorithm
                          // ============================

class SipHashAlgorithm
{
  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 uint64;

    // DATA
    uint64 d_v0;
    uint64 d_v1;
    uint64 d_v2;
    uint64 d_v3;
    unsigned char d_buf [8];
    unsigned int d_bufSize;
    unsigned int d_totalLength;

    // PRIVATE MANIPULATORS
    void init();
        // Initialize the state of this object

  public:
    // TYPES
    typedef uint64 result_type;
        // Typedef indicating the size of the hash this algorithm will return.

    // CONSTANTS
    enum { k_SEED_LENGTH = 8 };     // Seed length in bytes

    // CREATORS
    explicit SipHashAlgorithm(const char *seed);
        // Create a 'SipHashAlgorithm' with a 128-bit ('k_SEED_LENGTH' bytes)
        // seed pointed to by the specified 'seed'. Note that if data in 'seed'
        // is not random, all gaurentees of security and denial of service
        // protection are void.

    // MANIPULATORS
    void operator()(void const* key, size_t len);
        // Incorporates the specified 'key' of 'len' bytes into the internal
        // state of the hashing algorithm. Input where 'len' == 0 will have no
        // effect on the internal state of the algorithm.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash' multiple times in a row will return different results,
        // and only the first result returned will match the expected result of
        // the algorithm.
};

}  // close namespace bslh

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SipHashAlgorithm>
    : bsl::true_type {};
}  // close namespace bslmf

}  // close namespace BloombergLP

#endif

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
