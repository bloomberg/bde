// bslh_defaulthashalgorithm.h                                        -*-C++-*-
#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#define INCLUDED_BSLH_DEFAULTHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typedef for a reasonable default hashing algorithm.
//
//@CLASSES:
// bslh::DefaultHashAlgorithm: Typedef for a good default hashing algorithm
//
//@SEE_ALSO: bslh_hash, bslh_securehashalgorithm
//
//@DESCRIPTION: 'bslh::DefaultHashAlgorithm' provides a typedef far a good
// default hashing algorithm, suitable for producing hashes for a hash table.
//
///Properties
///----------
// The following describe the extent to which different properties can be
// expected from a default hashing algorithm.
//
///Security
/// - - - -
// There are NO security guarantees made by 'bslh::DefaultHashAlgorithm'. If
// security is required, look at 'bslh::SecureHashAlgorithm'.
//
///Speed
///- - -
// The default hash algorithm will produce hashes fast enough to be applicable
// for general purpose use.
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
// The default hash algorithm only guarantees that hashes will remain
// consistent within a single process. This means different hashes may be
// produced on machines of different endianness or even between runs on the
// same machine. Therefor it is not recommended to send hashes from
// 'bslh::DefaultHashAlgorithm' over a network. It is also not recommended to
// write hashes from 'bslh::DefaultHashAlgorithm' to shared memory or the disk.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_SPOOKYHASHALGORITHM
#include <bslh_spookyhashalgorithm.h>
#endif

namespace BloombergLP {

namespace bslh {

#pragma bde_verify -TR17
typedef SpookyHashAlgorithm DefaultHashAlgorithm;
#pragma bde_verify +TR17

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
