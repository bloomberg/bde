// btls_iovecutil.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls_iovecutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls_iovecutil_cpp,"$Id$ $CSID$")

#include <btlb_blob.h>
#include <btlb_pooledblobbufferfactory.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace btls {
namespace {

                         // ------------------------
                         // local function templates
                         // ------------------------

template <class IOVEC>
void genericAppendToBlob(btlb::Blob  *blob,
                         const IOVEC *vectors,
                         int          numVectors,
                         int          offset)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <  numVectors);

    // Set up loop invariant stated below.  Note that if blobLength is 0, or if
    // last data buffer is complete, the call 'blob->setLength(...)' below will
    // create additional buffers as needed, so that the call to
    // 'blob->buffer(currentBufIndex)' will always be legal.

    int currentBufIndex  = blob->numDataBuffers() - 1;
    int currentBufOffset = blob->lastDataBufferLength();
    if (currentBufIndex < 0 ||
        currentBufOffset == blob->buffer(currentBufIndex).size()) {

        // Blob is empty or last data buffer is complete: skip to next buffer.

        ++currentBufIndex;
        currentBufOffset = 0;
    }

    // Skip to 'offset' in 'vectors'.

    int currentVecIndex = 0;
    int currentVecAvailable = vectors[currentVecIndex].length();
    BSLS_ASSERT(0 < currentVecAvailable);

    int prefixLength = 0;
    while (prefixLength + currentVecAvailable <= offset) {
        prefixLength += currentVecAvailable;
        ++currentVecIndex;
        currentVecAvailable = vectors[currentVecIndex].length();
        BSLS_ASSERT(0 < currentVecAvailable);
    }
    int currentVecOffset = offset - prefixLength;
    currentVecAvailable -= currentVecOffset;
    BSLS_ASSERT(0 <= currentVecOffset);
    BSLS_ASSERT(0 < currentVecAvailable);

    // For simplicity, finish computing the iovec lengths, and reserve blob's
    // length in a single setLength call.  Since prefixLength isn't used, we
    // reset it and use it for that computation.

    prefixLength = currentVecAvailable;
    for (int i = currentVecIndex+1; i < numVectors; ++i) {
        prefixLength += vectors[i].length();
    }
    blob->setLength(blob->length() + prefixLength);

    // Compute number of bytes available to read or write in current vector or
    // buffer.  This must be done *after* setting the blob length.

    int currentBufAvailable = blob->buffer(currentBufIndex).size();
    currentBufAvailable -= currentBufOffset;
    BSLS_ASSERT(0 < currentBufAvailable);

    // Append the iovecs as individual blob buffers, re-segmented to take
    // advantage of the factory's buffer size: this is a classic merge.

    while (1) {
        // Invariants:
        //: 1.  0 <= currentVec < numVectors
        //: 2.  0 <= currentVecOffset < vectors[currentVec].length()
        //: 3.  0 <  currentVecAvailable
        //: 4.  0 <= currentBuf < blobs.numDataBuffers()
        //: 5.  0 <= currentBufOffset < blob->buffer(currentBuf).size()
        //: 6.  0 <  currentBufAvailable

        int numBytesCopied = bsl::min(currentVecAvailable,
                                      currentBufAvailable);

        bsl::memcpy(blob->buffer(currentBufIndex).data() + currentBufOffset,
                    static_cast<const char *>(
                                             vectors[currentVecIndex].buffer())
                                                            + currentVecOffset,
                    numBytesCopied);

        currentBufOffset += numBytesCopied;
        currentVecOffset += numBytesCopied;

        if (currentVecAvailable == numBytesCopied) {
            currentVecOffset = 0;
            if (++currentVecIndex == numVectors) {
                return;                                               // RETURN
            }
            currentVecAvailable = vectors[currentVecIndex].length();
        } else {
            currentVecAvailable -= numBytesCopied;
        }

        if (currentBufAvailable == numBytesCopied) {
            currentBufOffset = 0;
            ++currentBufIndex;
            currentBufAvailable = blob->buffer(currentBufIndex).size();
        } else {
            currentBufAvailable -= numBytesCopied;
        }
    }
}

template <class IOVEC>
int genericGather(char        *buffer,
                  int          length,
                  const IOVEC *buffers,
                  int          numBuffers)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);

    int numCopied = 0;
    while(numBuffers && 0 < length) {
        if (buffers->length() < length) {
            bsl::memcpy(buffer,
                        static_cast<const char *>(buffers->buffer()),
                        buffers->length());
            numCopied += buffers->length();
            length -= buffers->length();
            buffer += buffers->length();
            ++buffers;
        }
        else {
            bsl::memcpy(buffer,
                        static_cast<const char *>(buffers->buffer()),
                        length);
            numCopied += length;
            length = 0;
        }
        --numBuffers;
    }
    return numCopied;
}

template <class IOVEC>
int genericLength(const IOVEC *buffers, int numBuffers)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);

    int length = 0;
    while (numBuffers) {
        length += buffers->length();
        ++buffers;
        --numBuffers;
    }
    return length;
}

template <class IOVEC>
void genericPivot(int         *bufferIndex,
                  int         *offset,
                  const IOVEC *buffers,
                  int          numBuffers,
                  int          position)
{
    BSLS_ASSERT(bufferIndex);
    BSLS_ASSERT(offset);
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(0 <= position);

    for (int i = 0; i < numBuffers; ++i) {
        if (position < buffers[i].length()) {
            *bufferIndex = i;
            *offset    = position;
            return;                                                   // RETURN
        }
        position -= buffers[i].length();
    }

    *bufferIndex = numBuffers;
    return;
}

template <class IOVEC>
int genericScatter(const IOVEC *buffers,
                   int          numBuffers,
                   const char  *buffer,
                   int          length)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    int numCopied = 0;
    while(numBuffers && 0 < length) {
        if (buffers->length() < length) {
            bsl::memcpy(static_cast<char *>(
                                        const_cast<void *>(buffers->buffer())),
                        buffer,
                        buffers->length());
            numCopied += buffers->length();
            length    -= buffers->length();
            buffer    += buffers->length();
            ++buffers;
        }
        else {
            bsl::memcpy(static_cast<char *>(
                                        const_cast<void *>(buffers->buffer())),
                        buffer,
                        length);
            numCopied += length;
            length = 0;
        }
        --numBuffers;
    }
    return numCopied;
}

}  // close unnamed namespace

                             // ---------------
                             // class IovecUtil
                             // ---------------

void IovecUtil::appendToBlob(btlb::Blob  *blob,
                             const Iovec *buffers,
                             int          numBuffers,
                             int          offset)
{
    genericAppendToBlob(blob, buffers, numBuffers, offset);
}

void IovecUtil::appendToBlob(btlb::Blob *blob,
                             const Ovec *buffers,
                             int         numBuffers,
                             int         offset)
{
    genericAppendToBlob(blob, buffers, numBuffers, offset);
}

btlb::Blob *IovecUtil::blob(const Iovec             *buffers,
                            int                      numBuffers,
                            int                      offset,
                            btlb::BlobBufferFactory *factory,
                            bslma::Allocator        *basicAllocator)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);
    btlb::Blob       *blob = new (*allocator) btlb::Blob(factory, allocator);

    appendToBlob(blob, buffers, numBuffers, offset);
    return blob;
}

int IovecUtil::gather(char        *buffer,
                      int          length,
                      const Iovec *buffers,
                      int          numBuffers)
{
    return genericGather(buffer, length, buffers, numBuffers);
}

int IovecUtil::gather(char       *buffer,
                      int         length,
                      const Ovec *buffers,
                      int         numBuffers)
{
    return genericGather(buffer, length, buffers, numBuffers);
}

int IovecUtil::length(const Iovec *buffers, int numBuffers)
{
    return genericLength(buffers, numBuffers);
}

int IovecUtil::length(const Ovec *buffers, int numBuffers)
{
    return genericLength(buffers, numBuffers);
}

void IovecUtil::pivot(int         *bufferIndex,
                      int         *offset,
                      const Iovec *buffers,
                      int          numBuffers,
                      int          position)
{
    genericPivot(bufferIndex, offset, buffers, numBuffers, position);
}

void IovecUtil::pivot(int        *bufferIndex,
                      int        *offset,
                      const Ovec *buffers,
                      int         numBuffers,
                      int         position)
{
    genericPivot(bufferIndex, offset, buffers, numBuffers, position);
}

int IovecUtil::scatter(const Iovec *buffers,
                       int          numBuffers,
                       const char  *buffer,
                       int          length)
{
    return genericScatter(buffers, numBuffers, buffer, length);
}

int IovecUtil::scatter(const Ovec *buffers,
                       int         numBuffers,
                       const char *buffer,
                       int         length)
{
    return genericScatter(buffers, numBuffers, buffer, length);
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
