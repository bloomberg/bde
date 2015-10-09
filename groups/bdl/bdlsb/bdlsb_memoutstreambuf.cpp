// bdlsb_memoutstreambuf.cpp                                          -*-C++-*-
#include <bdlsb_memoutstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_memoutstreambuf_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace bdlsb {

                           // ---------------------
                           // class MemOutStreamBuf
                           // ---------------------

// PRIVATE MANIPULATORS
void MemOutStreamBuf::grow(size_t newLength)
{
    enum { k_MAX_PRE_GROW = INT_MAX / k_GROWTH_FACTOR };

    bsl::size_t newCapacity = capacity();
    if (0 == newCapacity) {
        newCapacity = k_INITIAL_BUFFER_SIZE;
    }

    if (newCapacity < newLength) {
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newLength > k_MAX_PRE_GROW))
        {
            newCapacity = (INT_MAX/2 + newLength/2) | 1;
        }
        else {
            do {
                newCapacity *= k_GROWTH_FACTOR;
            } while (newCapacity < newLength);
        }
    }

    BSLS_ASSERT_SAFE(newCapacity >= newLength);

    reserveCapacity(newCapacity);
}

// PROTECTED MANIPULATORS
int MemOutStreamBuf::overflow(int_type insertionChar)
{
    if (traits_type::eof() == insertionChar) {
        return traits_type::not_eof(insertionChar);                   // RETURN
    }

    grow(capacity() + 1);
    return sputc(static_cast<char_type>(insertionChar));
}

MemOutStreamBuf::pos_type
MemOutStreamBuf::seekoff(MemOutStreamBuf::off_type offset,
                               bsl::ios_base::seekdir          fixedPosition,
                               bsl::ios_base::openmode         which)
{
    // This is an output-only buffer, so cannot "seek" in "get" area.

    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);                                          // RETURN
    }

    // Compute offset from current position.  In this stream, 'pptr()' defines
    // both the current position and the end of the logical byte stream.  Thus,
    // 'bsl::ios_base::curr' and 'bsl::ios_base::end' are handled identically.

    off_type currOffset = bsl::ios_base::beg == fixedPosition
                          ? offset - length()
                          : offset;

    // 'currOffset' is invalid if it is positive or has an absolute-value
    // greater than 'length()'.

    if (currOffset > 0 || -currOffset > length()) {
        return pos_type(-1);                                          // RETURN
    }

    pbump(static_cast<int>(currOffset));

    return pos_type(length());
}

MemOutStreamBuf::pos_type
MemOutStreamBuf::seekpos(MemOutStreamBuf::pos_type position,
                               bsl::ios_base::openmode         which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

bsl::streamsize MemOutStreamBuf::xsputn(const char_type *source,
                                        bsl::streamsize  numChars)
{
    BSLMF_ASSERT(bsl::numeric_limits<bsl::streamsize>::is_signed);
    BSLS_ASSERT(0 <= numChars);

    const bsl::size_t newLength = static_cast<bsl::size_t>(
                                                          length() + numChars);
    BSLS_ASSERT_SAFE(newLength >= length());

    if (newLength > capacity()) {
        grow(newLength);
    }
    bsl::copy(source, source + numChars, pptr());

    // 'pbump' accepts an 'int' so (in the unlikely case) that
    // 'remainingLength' is greater than INT_MAX, we must call 'pbump' multiple
    // times.

    const bsl::streamsize intMax = static_cast<bsl::streamsize>(
                                              bsl::numeric_limits<int>::max());
    bsl::streamsize remainingLength = numChars;
    do {
        int bumpLength = static_cast<int>(bsl::min(intMax, remainingLength));
        pbump(bumpLength);
        remainingLength -= bumpLength;
    } while (remainingLength > 0);

    return numChars;
}

// MANIPULATORS
void MemOutStreamBuf::reserveCapacity(bsl::size_t numCharacters)
{
    if (numCharacters <= capacity()) {
        return;                                                       // RETURN
    }

    char_type *oldBuffer = pbase();
    char_type *newBuffer = static_cast<char_type *>(
                                       d_allocator_p->allocate(numCharacters));

    // Copy over existing characters.

    const bsl::size_t oldLength = length();
    bsl::copy(oldBuffer, oldBuffer + oldLength * sizeof(char_type), newBuffer);
    d_allocator_p->deallocate(oldBuffer);

    // Reset data members appropriately.
    setp(newBuffer, newBuffer + numCharacters);

    // 'pbump' accepts an 'int' so (in the unlikely case) that
    // 'remainingLength' is greater than INT_MAX, we must call 'pbump' multiple
    // times.

    const bsl::size_t intMax = static_cast<bsl::size_t>(
                                              bsl::numeric_limits<int>::max());

    bsl::size_t remainingLength = oldLength;
    do {
        int bumpLength = static_cast<int>(bsl::min(intMax, remainingLength));
        pbump(bumpLength);
        remainingLength -= bumpLength;
    } while (remainingLength > 0);
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
