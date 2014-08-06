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
// bslh::DefaultHashAlgorithm: A default hashing algorithm.
//
//@SEE_ALSO: bslh_hash, bslh_securehashalgorithm,
// bslh_defaultseededhashalgorithm
//
//@DESCRIPTION: 'bslh::DefaultHashAlgorithm' provides an unspecified default
// hashing algorithm. The supplied algorithm is suitable for general purpose
// use in a hash table. The underlying algorithm is subject to change in future
// releases.
//
///Properties
///----------
// The extent to which different properties can be expected from a default
// hashing algorithm are described as follows:
//
///Security
/// - - - -
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker. Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are NO security guarantees made by 'bslh::DefaultHashAlgorithm'. If
// security is required, look at 'bslh::SipHashAlgorithm'.
//
///Speed
///- - -
// The default hash algorithm will comput the hash on the order of O(n) where n
// is the length of the input data. Note that the default hash algorithm will
// produce hashes fast enough to be used for keying a hash table.
//
///Hash Distribution
///- - - - - - - - -
// The default hash algorithm will distribute hashes in a pseudorandom
// distribution across the key space. The hash function will exhibit avalanche
// behavior, meaning changing one bit of input will result in a 50% chance of
// each output bit changing. Avalanche behavior is enough to guarantee good key
// distribution, even when values are consecutive.
//
///Hash Consistency
/// - - - - - - - -
// The default hash algorithm guarantees only that hashes will remain
// consistent within a single process, meaning different hashes may be produced
// on machines of different endianness or even between runs on the same
// machine. Therefor it is not recommended to send hashes from
// 'bslh::DefaultHashAlgorithm' over a network. It is also not recommended to
// write hashes from 'bslh::DefaultHashAlgorithm' to any memory accessible by
// multiple machines.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#include <bslh_spookyhashalgorithm.h>
#endif

namespace BloombergLP {

namespace bslh {

class DefaultHashAlgorithm
{
    // Provides a default hashing algorithm that is appropriate for general
    // purpose use.

    // PRIVATE TYPES
    typedef bslh::SpookyHashAlgorithm InternalHashAlgorithm;
        // Typedef indicating the algorithm currently being used by
        // 'bslh::DefualtHashAlgorithm' to compute hashes. This algorithm is
        // subject to change.

    // DATA
    InternalHashAlgorithm d_state;
        // Object storing the state of the chosen 'InternalHashAlgorithm'.

  public:
    // TYPES
    typedef InternalHashAlgorithm::result_type result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CREATORS
    DefaultHashAlgorithm();
        // Create a 'bslh::DefaultHashAlgorithm', default constructing the
        // algorithm being used internally.

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
DefaultHashAlgorithm::DefaultHashAlgorithm()
: d_state()
{
}

// MANIPULATORS
inline
void DefaultHashAlgorithm::operator()(void const* key, size_t length)
{
    d_state(key, length);
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
