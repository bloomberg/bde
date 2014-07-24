// bslh_seedgenerator.h                                               -*-C++-*-
#ifndef INCLUDED_BSLH_SEEDGENERATOR
#define INCLUDED_BSLH_SEEDGENERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate arbitrary length seeds for algorithms.
//
//@CLASSES:
//  bslh::SeedGenerator: Arbitrary length seed generator
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a class, 'bslh::SeedGenerator', which
//  utilizes a user-supplied random number generator to generate arbitrary
//  length seeds. The quality of the seeds will only be as good as the quality
//  of the supplied RNG. A cryptographically secure RNG must be supplied in
//  order for 'SeedGenerator' to produce seeds suitable for a cryptographically
//  secure hashing algorithm.

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHMIMP
#include <bslh_spookyhashalgorithmimp.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // for 'memcpy'
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslh {

                        // =========================
                        // class bslh::SeedGenerator
                        // =========================
template<class RNG>
class SeedGenerator
{
    // Takes an initial 32-bit seed and uses it to generate an arbitrary length
    // seed.

  private:
    // PRIVATE TYPES
    typedef typename RNG::result_type result_type;
        // 'result_type' is an alias for the value returned by a call to
        // 'operator()' on the parameterized 'RNG'

    // DATA
    RNG          d_randomNumberGenerator; // User provided seed
    const size_t k_RNGOUTPUTSIZE;         // Size in bytes of the rng's output

  public:
    // CREATORS
    SeedGenerator();
        // Create a 'bslh::SeedGenerator' that will default construct the
        // parameterized 'RNG' and use it to generate its seeds.

    explicit SeedGenerator(RNG randomNumberGenerator);
        // Create a 'bslh::SeedGenerator' that will use the specified
        // 'randomNumberGenerator' to generate its seeds.

    // ACCESSORS
    void generateSeed(char *seedLocation, size_t seedLength);
        // Generate a seed of the specified 'seedLength' bytes and store it at
        // the specified 'seedLocation'. The seed will be generated with bytes
        // from the random number generator supplied at construction.
};

// CREATORS
template<class RNG>
SeedGenerator<RNG>::SeedGenerator() :
                         d_randomNumberGenerator(),
                         k_RNGOUTPUTSIZE(sizeof(typename RNG::result_type)) { }

template<class RNG>
SeedGenerator<RNG>::SeedGenerator(RNG randomNumberGenerator) :
                         d_randomNumberGenerator(randomNumberGenerator),
                         k_RNGOUTPUTSIZE(sizeof(typename RNG::result_type)) { }

// ACCESSORS
template<class RNG>
void SeedGenerator<RNG>::generateSeed(char *seedLocation, size_t seedLength) {
    size_t numChunks = seedLength / k_RNGOUTPUTSIZE;
    size_t remainder = seedLength % k_RNGOUTPUTSIZE;
    char  *chunkEnd  = seedLocation + numChunks * k_RNGOUTPUTSIZE;

    for(; seedLocation != chunkEnd; seedLocation += k_RNGOUTPUTSIZE) {
        *(reinterpret_cast<result_type *>(seedLocation)) =
                                                     d_randomNumberGenerator();
    }

    if(remainder) {
        result_type randomBytes = d_randomNumberGenerator();
        memcpy(seedLocation, &randomBytes, remainder);
    }
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
