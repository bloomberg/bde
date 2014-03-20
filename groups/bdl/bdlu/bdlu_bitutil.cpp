// bdlu_bitutil.cpp                                                   -*-C++-*-
#include <bdlu_bitutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlu_bitutil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

namespace BloombergLP {
namespace bdlu {

BSLMF_ASSERT(4 == sizeof(uint32_t));
BSLMF_ASSERT(8 == sizeof(uint64_t));

                        // --------------
                        // struct BitUtil
                        // --------------

// PRIVATE CLASS METHODS
int BitUtil::privateNumLeadingUnsetBits(uint32_t value)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    value |= value >> 16;
    value |= value >>  8;
    value |= value >>  4;
    value |= value >>  2;
    value |= value >>  1;
    return 32 - numBitsSet(value);
}

int BitUtil::privateNumLeadingUnsetBits(uint64_t value)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    value |= value >> 32;
    value |= value >> 16;
    value |= value >>  8;
    value |= value >>  4;
    value |= value >>  2;
    value |= value >>  1;
    return 64 - numBitsSet(value);
}

int BitUtil::privateNumTrailingUnsetBits(uint32_t value)
{
    value |= value << 16;
    value |= value <<  8;
    value |= value <<  4;
    value |= value <<  2;
    value |= value <<  1;
    return numBitsSet(~value);
}

int BitUtil::privateNumTrailingUnsetBits(uint64_t value)
{
    value |= value << 32;
    value |= value << 16;
    value |= value <<  8;
    value |= value <<  4;
    value |= value <<  2;
    value |= value <<  1;
    return numBitsSet(~value);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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
