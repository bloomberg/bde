// bslh_spookyhashalgorithm.h                                         -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#define INCLUDED_BSLH_SPOOKYHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the SpookyHash algorithm.
//
//@CLASSES:
//  bslh::SpookyHashAlgorithm: Functor implementing the SpookyHash algorithm
//
//@SEE_ALSO: bslh_hash, bslh_spookhashalgorithmimp
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithm' implements the SpookyHash algorithm
// by Bob Jenkins. This algorithm is a general purpose algorithm that is known
// to quickly reach good avalanche performance and execute in time that is
// comprable to or faster than other industry standard algorithms such as
// CityHash.  It is a good default choice for hashing values in unordered
// associative containers.  For more information, see:
// http://burtleburtle.net/bob/hash/spooky.html
//
// This class satisfies the requirements for regular 'bslh' hashing algorithms
// and seeded 'bslh' hashing algorithms, defined in bslh_hash.h and
// bslh_seededhash.h respectively.  More information about these requirements
// can also be found here:
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Using+Modular+Hashing&spaceKey=bde
//
///Security
///--------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker.  Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are NO security guarantees made by 'bslh::SpookyHashAlgorithm'.  If
// security is required, an algorithm that documents better secure properties
// should be used, such as 'bslh_siphashalgorithm'.
//
///Speed
///-----
// This algorithm will compute a hash on the order of O(n) where 'n' is the
// length of the input data.  Note that this algorithm will produce hashes fast
// enough to be used to hash keys in a hash table.  It is quicker than
// specialized algorithms such as SipHash, but not as fast as hashing using the
// identity function.
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
// This hash algorithm is endian-specific. It is designed for little endian
// machines, however, it will run on big endian machines.  On big endian
// machines, the Performance and Security Guarantees still apply, however the
// hashes produced will be different from those produced by the canonical
// implementation.  The creator of this algorithm acknowledges this and says
// that the big-endian hashes are just as good as the little-endian ones.  It
// is not recommended to send hashes from 'bslh::SpookyHashAlgorihtm' over a
// network becuase of the differances in hashes across architectures.  It is
// also not recommended to write hashes from 'bslh::SpookyHashAlgorihtm' to any
// memory accessible by multiple machines.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#include <bslh_spookyhashalgorithmimp.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

class SpookyHashAlgorithm {
    // This class wraps an implementation of the "SpookyHash" hash algorithm in
    // an interface that is usable in the modular hashing system in 'bslh' (see
    // https://cms.prod.bloomberg.com/team/display/bde/Modular+Hashing).

  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;
        // Typedef for a 64-bit integer type used in the hashing algorithm.

    // DATA
    SpookyHashAlgorithmImp d_state;
        // Object that contains the actual implementation of the SpookHash
        // algorithm.

    // NOT IMPLEMENTED
    SpookyHashAlgorithm(const SpookyHashAlgorithm& original); // = delete;
        // Do not allow copy construction.

    SpookyHashAlgorithm& operator=(const SpookyHashAlgorithm& rhs);// = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef bsls::Types::Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.


    // CONSTANTS
    enum { k_SEED_LENGTH = 16 }; // Seed length in bytes.

    // CREATORS
    SpookyHashAlgorithm();
        // Create a 'SpookyHashAlgorithm' using a default initial seed.

    explicit SpookyHashAlgorithm(const char *seed);
        // Create an instance of 'SpookyHashAlgorithm' seeded with a 128-bit
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.
        // Each bit of the supplied seed will contribute to the final hash
        // produced by 'computeHash()'. The behaviour is undefined unless
        // 'seed' points to at least 16 bytes of initialized memory.

    //! ~SpookyHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t length);
        // Incorporate the specified 'length' bytes of 'data' into the internal
        // state of the hashing algorithm. Every bit of data incorporated into
        // the internal state of the algorithm will contribute to the final
        // hash produced by 'computeHash()'. The same hash value will be
        // produced regardless of whether a sequence of bytes is passed in all
        // at once or through multiple calls to this member function. Input
        // where 'length' == 0 will have no effect on the internal state of the
        // algorithm. The behaviour is undefined unless 'data' points to at
        // least 'length' bytes of initialized memory.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash' multiple times in a row will return different results,
        // and only the first result returned will match the expected result of
        // the algorithm. Also note that a value will be returned, even if data
        // has not been passed into 'operator()'
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
SpookyHashAlgorithm::SpookyHashAlgorithm()
: d_state(1, 2)
{
}

inline
SpookyHashAlgorithm::SpookyHashAlgorithm(const char *seed)
: d_state(reinterpret_cast<const Uint64 *>(seed)[0],
          reinterpret_cast<const Uint64 *>(seed)[1])
{
}

// MANIPULATORS
inline
void SpookyHashAlgorithm::operator()(const void *data, size_t length)
{
    BSLS_ASSERT(data);

    d_state.update(data, length);
}

inline
SpookyHashAlgorithm::result_type SpookyHashAlgorithm::computeHash()
{
    bsls::Types::Uint64 h1, h2;
    d_state.finalize(&h1, &h2);
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
