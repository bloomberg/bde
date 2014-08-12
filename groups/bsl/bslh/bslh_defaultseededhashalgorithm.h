// bslh_defaultseededhashalgorithm.h                                  -*-C++-*-
#ifndef INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM
#define INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reasonable seeded hashing algorithm for default use.
//
//@CLASSES:
//  bslh::DefaultSeededHashAlgorithm: A default seeded hashing algorithm.
//
//@SEE_ALSO: bslh_hash, bslh_siphashalgorithm, bslh_defaulthashalgorithm
//
//@DESCRIPTION: 'bslh::DefaultSeededHashAlgorithm' provides an unspecified
// default seeded hashing algorithm.  The supplied algorithm is suitable for
// general purpose use in a hash table.  The underlying algorithm is subject to
// change in future releases.
//
// This class satisfies the requirements for seeded 'bslh' hashing algorithms,
// defined in bslh_seededhash.h.  More information about these requirements can
// also be found here:
// https://cms.prod.bloomberg.com/team/pages/viewpage.action?title=
// Using+Modular+Hashing&spaceKey=bde
//
///Security
///--------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker.  Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are NO security guarantees made by 'bslh::DefaultSeededHashAlgorithm'.
// If security is required, an algorithm that documents better secure
// properties should be used, such as 'bslh_siphashalgorithm'.
//
///Speed
///-----
// The default hash algorithm will compute a hash on the order of O(n) where
// 'n' is the length of the input data.  Note that this algorithm will produce
// hashes fast enough to be used to hash keys in a hash table.  The chosen
// algorithm will be quicker than specialized algorithms such as SipHash, but
// not as fast as hashing using the identity function.
//
///Hash Distribution
///-----------------
// The default hash algorithm will distribute hashes in a pseudorandom
// distribution across the key space.  The hash function will exhibit avalanche
// behavior, meaning changing one bit of input will result in a 50% chance of
// each output bit changing.  Avalanche behavior is enough to guarantee good
// key distribution, even when values are consecutive.
//
///Hash Consistency
///----------------
// The default hash algorithm guarantees only that hashes will remain
// consistent within a single process, meaning different hashes may be produced
// on machines of different endianness or even between runs on the same
// machine.  Therefor it is not recommended to send hashes from
// 'bslh::DefaultSeededHashAlgorithm' over a network.  It is also not
// recommended to write hashes from 'bslh::DefaultSeededHashAlgorithm' to any
// memory accessible by multiple machines.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#include <bslh_spookyhashalgorithm.h>
#endif

namespace BloombergLP {

namespace bslh {

class DefaultSeededHashAlgorithm {
    // This class wraps an unspecified default hashing algorithm, which takes a
    // seed, that is appropriate for general purpose use such as generating
    // hashes for a hash table. This class implements an interface usable in
    // the modular hashing system in 'bslh' (see
    // https://cms.prod.bloomberg.com/team/display/bde/Modular+Hashing)

  private:
    // PRIVATE TYPES
    typedef bslh::SpookyHashAlgorithm InternalHashAlgorithm;
        // Typedef indicating the algorithm currently being used by
        // 'bslh::DefualtHashAlgorithm' to compute hashes. This algorithm is
        // subject to change.

    // DATA
    InternalHashAlgorithm d_state;
        // Object storing the state of the chosen 'InternalHashAlgorithm'.

    // NOT IMPLEMENTED
    DefaultSeededHashAlgorithm(); // = delete;
        // Do not allow default construction. This class must be constructed
        // with a seed.

    DefaultSeededHashAlgorithm(const DefaultSeededHashAlgorithm& original);
                                                                   // = delete;
        // Do not allow copy construction.

    DefaultSeededHashAlgorithm& operator=(
                           const DefaultSeededHashAlgorithm& rhs); // = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef InternalHashAlgorithm::result_type result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CONSTANTS
    enum { k_SEED_LENGTH = InternalHashAlgorithm::k_SEED_LENGTH };
        // Seed length in bytes.

    // CREATORS
    explicit DefaultSeededHashAlgorithm(const char *seed);
        // Create an instance of 'DefaultSeededHashAlgorithm' seeded with a
        // 128-bit ('k_SEED_LENGTH' bytes) seed pointed to by the specified
        // 'seed'.  Each bit of the supplied seed will contribute to the final
        // hash produced by 'computeHash()'.  The behaviour is undefined unless
        // 'seed' points to at least 16 bytes of initialized memory.

    //! ~DefaultSeededHashAlgorithm() = default;
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
        // internal state of the algorithm. The behaviour is undefined unless
        // 'data' points to at least 'length' bytes of initialized memory.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash' multiple times in a row will return different results,
        // and only the first result returned will match the expected result of
        // the algorithm. Also note that a value will be returned, even if data
        // has not been passed into 'operator()'.
};

// CREATORS
inline
DefaultSeededHashAlgorithm::DefaultSeededHashAlgorithm(const char *seed)
: d_state(seed)
{
    BSLS_ASSERT(seed);        
}

// MANIPULATORS
inline
void DefaultSeededHashAlgorithm::operator()(const void *data, size_t length)
{
    BSLS_ASSERT(data);
    d_state(data, length);
}

inline
DefaultSeededHashAlgorithm::result_type
DefaultSeededHashAlgorithm::computeHash()
{
    return d_state.computeHash();
}

}  // close package namespace

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
