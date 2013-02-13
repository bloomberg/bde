// bslstl_hash.cpp                                                    -*-C++-*-
#include <bslstl_hash.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BDE_OMIT_INTERNAL_DEPRECATED // DEPRECATED

#include <stdio.h>  // for 'printf'

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

namespace bsl
{

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // DEPRECATED
std::size_t hash<const char *>::operator()(const char *x) const
{
    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;

        printf("ERROR: bsl::hash called for 'const char *',"
               " see {TEAM BDEI:STANDARD HASH<GO}\n");
    }

    unsigned long result = 0;

    for (; *x; ++x) {
        result = 5 * result + *x;
    }

    return std::size_t(result);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace bsl
// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
