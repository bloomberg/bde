// bslh_oneatatimehashalgorithm.h                                     -*-C++-*-
#ifndef INCLUDED_BSLH_ONEATATIMEHASHALGORITHM
#define INCLUDED_BSLH_ONEATATIMEHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hashing algorithm with decent performance for all data.
//
//@CLASSES:
//  bslh::OneAtATimeHashAlg: Generic hashing algorithm functor
//
//@SEE_ALSO:
//
//@DESCRIPTION: 'bslh::OneAtATimeHashAlg' implements the one-at-a-time hashing
// algorithm that is known to quickly reach good avalance performance and is a
// good choice for hashing for associative containers.

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslh { 


                          // ===================================
                          // class bslh::OneAtATimeHashAlgorithm
                          // ===================================

class OneAtATimeHashAlgorithm
{
    // This class implements the "one-at-a-time" hash algorithm (see
    // http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx)
  public:
    typedef size_t result_type;

  private:
    size_t d_state;

  public:
    OneAtATimeHashAlgorithm();
        // Initialize the internal state of the algorithm

    void operator()(void const* key, size_t length);
        // Incorporates the specified 'key' of 'length' bytes into the internal
        // state of the hashing algorithm.


    result_type getHash();
        // Finalize the hash that has been accumulated and return it.

};

OneAtATimeHashAlgorithm::OneAtATimeHashAlgorithm() : d_state(0) { }

void OneAtATimeHashAlgorithm::operator()(void const* key, size_t length)
{
    unsigned char const *p = static_cast<unsigned char const *>(key);

    for (int i = 0; i < length; i++ ) {
        d_state += p[i];
        d_state += (d_state << 10 );
        d_state ^= (d_state >> 6  );
    }

    d_state += (d_state << 3  );
    d_state ^= (d_state >> 11 );
    d_state += (d_state << 15 );
}

OneAtATimeHashAlgorithm::result_type OneAtATimeHashAlgorithm::getHash()
{
    return d_state;
}

}  // close namespace bslh

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
