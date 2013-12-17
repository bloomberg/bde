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
void *BufferAllocator::allocate(bsls::Types::size_type size)
{
    BSLS_ASSERT_SAFE(0 <= size);

    // Calculate the appropriate aligned offset

    const int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
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
