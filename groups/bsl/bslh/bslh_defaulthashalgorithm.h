// bslh_defaulthashalgorithm.h                                        -*-C++-*-
#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#define INCLUDED_BSLH_DEFAULTHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reasonable hashing algorithm for default use.
//
//@CLASSES:
//  bslh::DefaultHashAlgorithm: A default hashing algorithm
//
//@SEE_ALSO: bslh_hash, bslh_securehashalgorithm,
//           bslh_defaultseededhashalgorithm
//
//@DESCRIPTION: 'bslh::DefaultHashAlgorithm' provides an unspecified default
// hashing algorithm.  The supplied algorithm is suitable for general purpose
// use in a hash table.  The underlying algorithm is subject to change in
// future releases.
//
// This class satisfies the requirements for regular 'bslh' hashing algorithms,
// defined in bslh_hash.h.  More information can be found in the package level
// documentation for 'bslh' (internal users can also find information here
// {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Security
///--------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker.  Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are NO security guarantees made by 'bslh::DefaultHashAlgorithm',
// meaning attackers may be able to engineer keys that will cause a DOS attack
// in hash tables using this algorithm.  If security is required, an algorithm
// that documents better secure properties should be used, such as
// 'bslh_siphashalgorithm'.
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
// 'bslh::DefaultHashAlgorithm' over a network.  It is also not recommended to
// write hashes from 'bslh::DefaultHashAlgorithm' to any memory accessible by
// multiple machines.

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

class DefaultHashAlgorithm {
    // This class wraps an unspecified default hashing algorithm that is
    // appropriate for general purpose use such as generating hashes for a hash
    // table.

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
    DefaultHashAlgorithm(const DefaultHashAlgorithm& original); // = delete;
        // Do not allow copy construction.

    DefaultHashAlgorithm& operator=(const DefaultHashAlgorithm& rhs);
                                                                   // = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef InternalHashAlgorithm::result_type result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CREATORS
    DefaultHashAlgorithm();
        // Create a 'bslh::DefaultHashAlgorithm', default constructing the
        // algorithm being used internally.

    //! ~DefaultHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes', into the internal state of the hashing algorithm.  Every
        // bit of data incorporated into the internal state of the algorithm
        // will contribute to the final hash produced by 'computeHash()'.  The
        // same hash will be produced regardless of whether a sequence of bytes
        // is passed in all at once or through multiple calls to this member
        // function.  Input where 'length' == 0 will have no effect on the
        // internal state of the algorithm.  The behaviour is undefined unless
        // 'data' points to at least 'length' bytes of initialized memory.


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
DefaultHashAlgorithm::DefaultHashAlgorithm()
: d_state()
{
}

// MANIPULATORS
inline
void DefaultHashAlgorithm::operator()(const void *data, size_t length)
{
    BSLS_ASSERT(data);
    d_state(data, length);
}

inline
DefaultHashAlgorithm::result_type DefaultHashAlgorithm::computeHash()
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
