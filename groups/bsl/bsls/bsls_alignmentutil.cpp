// bsls_alignmentutil.cpp                                             -*-C++-*-
#include <bsls_alignmentutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttest.h>  // for testing only
#include <bsls_types.h>       // for testing only

namespace BloombergLP {
namespace bsls {

template <int INTEGER>
struct AlignmentUtil_Assert;
    // This parameterized 'struct' is declared but not defined except for the
    // single specialization below.  It is used with the 'sizeof' operator to
    // verify the assumption that 'BSLS_MAX_ALIGNMENT' is a positive, integral
    // power of 2.  Note that 'bslmf_assert' cannot be used in 'bsls'.

template <>
struct AlignmentUtil_Assert<1> {
    // Applying 'sizeof' to this specialization will allow compilation to
    // succeed (i.e., the associated compile-time assert will succeed).

    enum { VALUE = 1 };
};

// Assert, at compile time, that 'BSLS_MAX_ALIGNMENT' is a positive, integral
// power of 2.

enum {

    assertion1 = sizeof(AlignmentUtil_Assert<
                                1 <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT>),

    assertion2 = sizeof(AlignmentUtil_Assert<
                           0 <= (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
                             & (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1))>)

};

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
