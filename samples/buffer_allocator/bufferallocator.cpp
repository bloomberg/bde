// bufferallocator.cpp                                                -*-C++-*-
#include <bufferallocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace Enterprise {
namespace pkg {

                        // ---------------------
                        // class BufferAllocator
                        // ---------------------

// MANIPULATORS
void *BufferAllocator::allocate(bsls_Types::size_type size)
{
    BSLS_ASSERT_SAFE(0 <= size);

    // Calculate the appropriate aligned offset

    const int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                                  d_buffer_p + d_cursor,
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

    if (d_cursor + offset + size > d_bufferSize) {
        return d_allocator_p->allocate(size);                         // RETURN
    }

    void *result = static_cast<void *>(&d_buffer_p[d_cursor + offset]);
    d_cursor += offset + size;

    return result;
}

void BufferAllocator::deallocate(void *address)
{
    if (!(d_buffer_p <= address && address < d_buffer_p + d_bufferSize)) {
        d_allocator_p->deallocate(address);
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
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
