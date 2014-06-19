// bslh_defaulthashalgorithm.h                                        -*-C++-*-
#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#define INCLUDED_BSLH_DEFAULTHASHALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a typedef for a reasonable deafult hashing algorithm.
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@DESCRIPTION: 'bslh::DefaultHashAlgorithm' provides a typedef for
// 'bslh::OneAtATimeHashAlgorithm'. This is done as a way to make it easier for
// users to find the appropriate hashing algorithm. It is unlikely for users to
// know whether 'bslh::OneAtATimeHashAlgorithm' is a good defualt choice, so
// 'bslh::DefaultHashAlgorithm' is supplied to make this more clear. This also
// allows us to swap out this hash algorithm for a new algorithm is a better
// defualt is found.

#ifndef INCLUDED_BSLH_ONEATATIMEHASHALGORITHM
#include <bslh_oneatatimehashalgorithm.h>
#endif

namespace BloombergLP {

namespace bslh { 

typedef OneAtATimeHashAlgorithm DefaultHashAlgorithm;

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
