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
//@DESCRIPTION: 'bslh::SipHashAlgorithm' implements the SipHash algorithm,
//  which is almost as fast as general purpose algorithms like SpookyHash, but
//  can offer cryptographically strong output. Given a cryptographically secure
//  seed, this pseudo random hash function will produce a hash distribution
//  that is indistinguishable from random. This hash algorithm can help
//  mitigate delial of service attacks on a hash table.


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


//------------------------------- siphash.h ------------------------------------
// 
// This software is in the public domain.  The only restriction on its use is
// that no one can remove it from the public domain by claiming ownership of it,
// including the original authors.
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
//------------------------------------------------------------------------------

namespace BloombergLP {

namespace bslh {


                          // ============================
                          // class bslh::SipHashAlgorithm
                          // ============================

class SipHashAlgorithm
{
  private:
    typedef bsls::Types::Uint64 uint64;
    uint64 d_v0;
    uint64 d_v1;
    uint64 d_v2;
    uint64 d_v3;
    unsigned char d_buf [8];
    unsigned int d_bufSize;
    unsigned int d_totalLength;

    void init();
        // Initialize the state of this object

  public:
    typedef uint64 result_type;

    SipHashAlgorithm();
    explicit SipHashAlgorithm(uint64 k0, uint64 k1 = 0);
        // Initialize the internal state of the algorithm

    void operator()(void const* key, size_t len);
        // Incorporates the specified 'key' of 'length' bytes into the internal
        // state of the hashing algorithm.

    result_type getHash();
        // Finalize the hash that has been accumulated and return it.
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
