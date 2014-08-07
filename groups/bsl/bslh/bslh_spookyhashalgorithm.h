// bslh_spookyhashalgorithm.h                                         -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#define INCLUDED_BSLH_SPOOKYHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide and implementation of the SpookyHash algorithm.
//
//@CLASSES:
// bslh::SpookyHashAlgorithm: Spooky hash algorithm.
//
//@SEE_ALSO: bslh_hash, bslh_spookhashalgorithmimp
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithm' implements the spooky hashing
//  algorithm by Bob Jenkins. The algorithm is a general purpose algorithm
//  that is known to quickly reach good avalanche performance and is very fast.
//  It is a good choice to use as a default hashing algorithm for associative
//  containers. See: http://burtleburtle.net/bob/hash/spooky.html
//
///Security Guarantees
///-------------------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker. Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are NO security guarantees made by 'bslh::SpookyHashAlgorithm'. If
// security is required, look at 'bslh::SipHashAlgorithm'.
//
///Performance
///-----------
// This hashing algorithm is a fast, general purpose algorithm. It is quicker
// than specialized algorithms such as SipHash, but not as fast as identity
// hashing. Output hashes will be well distributed and will avalanche, which
// means changing one bit on the input will completely change the output. This
// will prevent similar values from funneling to the same hash or bucket.
//
///Endianness
///----------
// This hash is endian-specific. It is designed for little endian machines,
// however, it will run on big endian machines. On big endian machines, the
// Performance and Security Guarantees still apply, however the hashes produced
// will be different from those produced by the canonical implementation. The
// creator of this algorithm acknowledges this and says that the big-endian
// hashes are just as good as the little-endian ones. Therefor it is not
// recommended to send hashes from 'bslh::SpookyHashAlgorihtm' over a network.
// It is also not recommended to write hashes from 'bslh::SpookyHashAlgorihtm'
// to any memory accessible by multiple machines.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#include <bslh_spookyhashalgorithmimp.h>
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


                        // ===============================
                        // class bslh::SpookyHashAlgorithm
                        // ===============================

class SpookyHashAlgorithm
    // This class wraps an implementation of the "SpookyHash" hash algorithm
    // (see http://burtleburtle.net/bob/hash/spooky.html).
{

    // PRIVATE TYPES
    typedef bsls::Types::Uint64 uint64;// 64-bit int type.

    // DATA
    SpookyHashAlgorithmImp d_state;   // Representation of SpookHash algorithm.

  public:
    // TYPES
    typedef bsls::Types::Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.


    // CONSTANTS
    enum { k_SEED_LENGTH = 16 }; // Seed length in bytes.

    // CREATORS
    SpookyHashAlgorithm();
        // Create a 'SpookyHashAlgorithm' with default seed values.

    explicit SpookyHashAlgorithm(const char *seed);
        // Create an instance of 'SpookyHashAlgorithm' seeded with a 128-bit
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.

    // MANIPULATORS
    void operator()(void const* key, size_t length);
        // Incorporates the specified 'key' of 'length' bytes into the internal
        // state of the hashing algorithm.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash' multiple times in a row will return different results,
        // and only the first result returned will match the expected result of
        // the algorithm.
};

// CREATORS
inline
SpookyHashAlgorithm::SpookyHashAlgorithm()
: d_state()
{
    d_state.Init(1, 2);
}

inline
SpookyHashAlgorithm::SpookyHashAlgorithm(const char *seed)
: d_state()
{
    const uint64 *seedPtr = reinterpret_cast<const uint64 *>(seed);
    d_state.Init(seedPtr[0], seedPtr[1]);
}

// MANIPULATORS
inline
void SpookyHashAlgorithm::operator()(void const* key, size_t length)
{
    d_state.Update(key, length);
}

inline
SpookyHashAlgorithm::result_type SpookyHashAlgorithm::computeHash()
{
    bsls::Types::Uint64 h1, h2;
    d_state.Final(&h1, &h2);
    return h1;
}

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SpookyHashAlgorithm>
    : bsl::true_type {};
}  // close package namespacemf

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
