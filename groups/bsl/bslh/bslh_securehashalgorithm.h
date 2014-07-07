// bslh_securehashalgorithm.h                                         -*-C++-*-
#ifndef INCLUDED_BSLH_SECUREHASHALGORITHM
#define INCLUDED_BSLH_SECUREHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typedef for a secure hashing algorithm.
//
//@CLASSES:
// bslh::SecureHashAlgorithm: Typedef for a secure hashing algorithm
//
//@SEE_ALSO: bslh_hash, bslh_oneatatimehashalgorithm
//
//@DESCRIPTION: 'bslh::SecureHashAlgorithm' provides a typedef for a secure
//  hashing algorithm. Given a random seed, this algorithm will act as a
//  psuedorandom function and produce hashes that are distributed in a way that
//  is indistinguishable from random. This hash algorithm may operate more
//  slowly than 'bslh::DefaultHashAlgorithm', however it will help mitigate the
//  risk of denial of service attacks on hash tables containing potentially
//  malicious input. For more information on hash table denial of service
//  attacks, see: https://www.nruns.com/_downloads/advisory28122011.pdf

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_SIPHASHALGORITHM
#include <bslh_siphashalgorithm.h>
#endif

namespace BloombergLP {

namespace bslh {

#pragma bde_verify -TR17
typedef SipHashAlgorithm SecureHashAlgorithm;
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
