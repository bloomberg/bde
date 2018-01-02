// bdlb_randomdevice.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_RANDOMDEVICE
#define INCLUDED_BDLB_RANDOMDEVICE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a common interface to a system's random number generator.
//
//@CLASSES:
//  bdlb::RandomDevice: namespace for system specific random-number generators.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a namespace, 'bdlb::RandomDevice', for
// a suite of functions used to generate random seeds from platform-dependent
// random number generators.  Two variants are provided: one which may block,
// but which potentially samples from a stronger distribution, and another
// which does not block, but which potentially should not be used for
// cryptography.  The strength of these random numbers and the performance of
// these calls is strongly dependent on the underlying system.  On UNIX-like
// platforms 'getRandomBytes()' reads from '/dev/random' and
// 'getRandomBytesNonBlocking()' reads from '/dev/urandom'.  On Windows both
// methods use 'CryptGenRandom'.
//
// Note that (at least on UNIX-like systems) it is not appropriate to call
// these functions repeatedly to generate many random numbers.  A call to
// 'getRandomBytes()' can block if available entropy is exhausted, and both
// 'getRandomBytes()' and 'getRandomBytesNonBlocking()' open and close their
// respective devices on each call.  Instead, these functions should be used
// for seeding pseudo-random random number generators.  (E.g., promiscuous use
// of 'getRandomBytes()' appears to have caused the WP in '{DRQS 92851043}'.)
//
// There is discussion about which of '/dev/random' or '/dev/urandom' is best,
// especially on modern Linux systems, while 'man' pages on other UNIX systems
// continue to make claims about '/dev/random' providing "more secure" numbers
// than '/dev/urandom'.  See '{http://www.2uo.de/myths-about-urandom/}', for
// example.  This component deliberately takes no stand on the issue, making
// both available and leaving it for users to decide which to use.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Seeding the Random-Number Generator
/// - - - - - - - - - - - - - - - - - - - - - - -
// System-provided random-number generators generally must be initialized with
// a seed value from which they go on to produce their stream of pseudo-random
// numbers.  We can use 'RandomDevice' to provide such a seed.
//
// First, we obtain the results of invoking the random-number generator without
// having seeded it:
//..
//  int unseededR1 = rand();
//  int unseededR2 = rand();
//..
// Then, we obtain a random number:
//..
//  int seed = 0;
//  int status = bdlb::RandomDevice::getRandomBytes(
//      reinterpret_cast<unsigned char *>(&seed), sizeof(seed));
//  assert(0 == status);
//  assert(0 != seed);    // This will fail every few billion attempts...
//..
// Next, we seed the random-number generator with our seed:
//..
//  srand(seed);
//..
// Finally, we observe that we obtain different numbers:
//..
//  assert(unseededR1 != rand());
//  assert(unseededR2 != rand());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlb {

                        // ===================
                        // struct RandomDevice
                        // ===================

struct RandomDevice {
    // This 'struct' provides a namespace for a suite of functions used for
    // acquiring random numbers from the system.

    // TYPES
    typedef bsls::Types::size_type size_t;       // for brevity of name

    // CLASS METHODS
    static int getRandomBytes(unsigned char *buffer, size_t numBytes);
        // Read the specified 'numBytes' from the system random number
        // generator into the specified 'buffer'.  Returns 0 on success,
        // non-zero otherwise.  Note that this method may block if called
        // repeatedly or if 'numBytes' is high.

    static int getRandomBytesNonBlocking(unsigned char *buffer,
                                         size_t         numBytes);
        // Read the specified 'numBytes' from the system non-blocking random
        // number generator into the specified 'buffer'.  Returns 0 on success,
        // non-zero otherwise.  Note that the cryptographic strength of samples
        // drawn from this pool may or may not be lower than that of those
        // drawn from the blocking pool, and may vary by platform.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
