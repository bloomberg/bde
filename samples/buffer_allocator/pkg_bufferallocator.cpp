// pkg_bufferallocator.cpp                                            -*-C++-*-
#include <pkg_bufferallocator.h>

#include <bsls_alignmentutil.h>

namespace Enterprise {
namespace pkg {

                        // ---------------------
                        // class BufferAllocator
                        // ---------------------

// MANIPULATORS
void *BufferAllocator::allocate(BloombergLP::bsls::Types::size_type size)
{
    // Calculate the appropriate aligned offset.

    const int offset =
                    BloombergLP::bsls::AlignmentUtil::calculateAlignmentOffset(
                         d_buffer_p + d_cursor,
                         BloombergLP::bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

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
// Copyright 2013 Bloomberg Finance L.P.
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
