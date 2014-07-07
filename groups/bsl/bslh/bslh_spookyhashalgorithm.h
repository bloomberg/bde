// bslh_spookyhashalgorithm.h                                         -*-C++-*-
#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#define INCLUDED_BSLH_SPOOKYHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the spooky hash algorithm.
//
//@CLASSES:
// bslh::SpookyHashAlgorithm: Spooky hash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::SpookyHashAlgorithm' implements the spooky hashing
//  algorithm that is known to quickly reach good avalance performance and is a
//  good choice for hashing for associative containers. See:
//  http://burtleburtle.net/bob/hash/spooky.html

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#include <bslh_spookyhashalgorithmimp.h>
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


                          // ===============================
                          // class bslh::SpookyHashAlgorithm
                          // ===============================

class SpookyHashAlgorithm
{
    // This class implements the "SpookyHash" hash algorithm (see
    // http://burtleburtle.net/bob/hash/spooky.html)
  public:
    typedef bsls::Types::Uint64 result_type;

  private:
    SpookyHashAlgorithmImp d_state;

  public:
    SpookyHashAlgorithm(size_t seed1 = 1, size_t seed2 = 2);
        // Initialize the internal state of the algorithm

    void operator()(void const* key, size_t length);
        // Incorporates the specified 'key' of 'length' bytes into the internal
        // state of the hashing algorithm.


    result_type getHash();// const;
        // Finalize the hash that has been accumulated and return it.
};

SpookyHashAlgorithm::SpookyHashAlgorithm(size_t seed1, size_t seed2)
: d_state()
{
    d_state.Init(seed1, seed2);
}

void SpookyHashAlgorithm::operator()(void const* key, size_t length)
{
    d_state.Update(key, length);
}

SpookyHashAlgorithm::result_type SpookyHashAlgorithm::getHash()// const         TODO const?
{
    bsls::Types::Uint64 h1, h2;
    d_state.Final(&h1, &h2);
    return h1;
}

}  // close namespace bslh

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::SpookyHashAlgorithm>
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
