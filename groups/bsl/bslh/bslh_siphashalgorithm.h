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
//  bslh::SipHashAlgorithm: Functor implementing the SipHash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::SipHashAlgorithm' implements the SipHash algorithm.
// SipHash is an algorithm designed for speed and security.  A primary use case
// for this algorithm is to provide an extra line of defense in hash tables
// (such as the underlying implementation of 'unordered_map') against malicious
// input that could cause denial of service (DOS) attacks.  It is based on one
// of the finalists for the SHA-3 cryptographic hash standard.  Full details of
// the hash function can be found here: https://131002.net/siphash/siphash.pdf
// This particular implementation has been derived from Howard Hinnant's work
// here: https://github.com/HowardHinnant/hash_append/blob/master/siphash.h and
// as much of the original code as possible, including comment headers, has
// been preserved.
//
// This class satisfies the requirements for seeded 'bslh' hashing algorithms,
// defined in bslh_seededhash.h.  More information about these requirements can
// also be found here:
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Using+Modular+Hashing&spaceKey=bde
//
///Security
///--------
// SipHash is NOT a cryptographically secure hash.  In the paper linked above,
// the creators of this hash function describe it as "Cryptographically
// Strong", but explicitly avoid calling it cryptographically secure.  In order
// to be cryptographically secure, and algorithm must, among other things,
// provide "collision resistance".  "Collision resistance" means that it should
// be difficult to find two different messages m1 and m2 such that hash(m1) =
// hash(m2).  Because of the limited sized output (only 2^64 possibilities) and
// the fast execution time of the algorithm, it is feasible to find collisions
// by brute force, making the algorithm not cryptographically secure.
//
// SipHash IS, however, a cryptographically strong PRF (pseudorandom function).
// This means, assuming a properly random seed is given, the output of this
// algorithm will be indistinguishable from a uniform random distribution.
// This property is enough for the algorithm to be able to protect a hash table
// from malicious DOS attacks
//
///Denial of Service Protection
/// - - - - - - - - - - - - - -
// Given a cryptographically secure seed, this algorithm will produce hashes
// with a distribution that is indistinguishable from random.  This
// distribution means that there is no way for an attacker to predict which
// keys will cause collisions, meaning that this algorithm can help mitigate
// denial of service attacks on a hash table.  Denial of service attacks occur
// when an attacker deliberately degrades the performace of the hash table by
// inserting data that will collide to the same bucket, causing an average
// constant time lookup to become a linear search.  This protection is only
// effective if the seed provided is a cryptographically secure random number
// that is not availible to the attacker.
//
///Speed
///-----
// This algorithm is designed to be fast in comparison to other algorithms
// making similar guarantees.  It is still slower than other commonly accepted
// and used hashes such as SpookyHash.  This algorithm should only be used when
// protection from malicious input is required.  Otherwise, an algorithm that
// documents better performance properties should be used, such as
// 'bslh_spookyhashalgorithm'.
//
///Hash Distribution
///-----------------
// Output hashes will be well distributed and will avalanche, which means
// changing one bit of the input will change approximately 50% of the output
// bits.  This will prevent similar values from funneling to the same hash or
// bucket.
//
///Hash Consistency
///----------------
// This hash algorithm is endian-specific.  The algorithm will run on big and
// little endian machines and the above guarantees apply on both architectures,
// however, the hashes produced will be different.  Therefor it is not
// recommended to send hashes from 'bslh::SipHashAlgorithm' over a network.  It
// is also not recommended to write hashes from 'bslh::SipHashAlgorithm' to any
// memory accessible by multiple machines.
//
///Changes
///-------
// The third party code begins with the "siphash.h" header below, and continues
// until the TYPE TRAITS banner below.  Changes made to the original code
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
//:   'std::Uint64_t', explicit conversion operator, and an '= default'
//:   constructor.
//:
//: 6 Added typedef to replace removed 'std::Uint64_t'
//:
//: 7 Added 'computeHash' to replace the removed explicit conversion
//:
//: 8 Added 'k_SEED_LENGTH' and changed the constructor to accept a 'const char
//:   *'
//:
//: 9 Added includes and include guards
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

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif


namespace BloombergLP {

namespace bslh {

                          // ============================
                          // class bslh::SipHashAlgorithm
                          // ============================

class SipHashAlgorithm {
    // This class wraps an implementation of the "SipHash" algorithm in an
    // interface that is usable in the modular hashing system in 'bslh' (see
    // https://cms.prod.bloomberg.com/team/display/bde/Modular+Hashing).

  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;
        // Typedef for a 64-bit integer type used in the hashing algorithm.

    // DATA
    Uint64 d_v0;
    Uint64 d_v1;
    Uint64 d_v2;
    Uint64 d_v3;
        // Stores the intermediate state of the algorithm as values are
        // accumulated

    unsigned char d_buf [8];
        // Used to buffer data until we have enough to do a full round of
        // computation as specified by the algorithm.

    unsigned int d_bufSize;
        // The length of the data currently stored in the buffer.

    unsigned int d_totalLength;
        // The total length of all data that has been passed into the
        // algorithm.

    // NOT IMPLEMENTED
    SipHashAlgorithm(); // = delete;
        // Do not allow default construction. This class must be constructed
        // with a seed.

    SipHashAlgorithm(const SipHashAlgorithm& original); // = delete;
        // Do not allow copy construction.

    SipHashAlgorithm& operator=(const SipHashAlgorithm& rhs); // = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CONSTANTS
    enum { k_SEED_LENGTH = 16 }; // Seed length in bytes.

    // CREATORS
    explicit SipHashAlgorithm(const char *seed);
        // Create an instance of 'SipHashAlgorithm' seeded with a 128-bit
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.
        // Each bit of the supplied seed will contribute to the final hash
        // produced by 'computeHash()'. The behavior is undefined unless 'seed'
        // points to an array of at least 16 'char's. Note that if data in
        // 'seed' is not random, all guarantees of security and denial of
        // service protection are void.

    //! ~SipHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t length);
        // Incorporate the specified 'length' bytes of 'data' into the
        // internal state of the hashing algorithm. Every bit of data
        // incorporated into the internal state of the algorithm will
        // contribute to the final hash produced by 'computeHash()'. The same
        // hash will be produced regardless of whether a sequence of bytes is
        // passed in all at once or through multiple calls to this member
        // function. Input where 'length' == 0 will have no effect on the
        // internal state of the algorithm.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash' multiple times in a row will return different results,
        // and only the first result returned will match the expected result of
        // the algorithm. Also note that a value will be returned, even if data
        // has not been passed into 'operator()'
};

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SipHashAlgorithm>
    : bsl::true_type {};
}  // close traits namespace

}  // close enterprise namespace

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
