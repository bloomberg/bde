// bdlbb_blobutil.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlbb_blobutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlbb_blobutil_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>

#include <bsl_c_ctype.h>
#include <bsl_iostream.h>

namespace BloombergLP {
namespace {

// HELPER FUNCTION
void copyFromPlace(char                *dstBuffer,
                   const bdlbb::Blob&   srcBlob,
                   bsl::pair<int, int>  place,
                   int                  length)
    // Copy the specified 'length' bytes, starting at the specified 'place' in
    // the specified 'srcBlob', to the specified 'dstBuffer'.  The behavior of
    // this function is undefined unless '0 < length', 'place' represents an
    // actual character position in 'srcBlob', 'srcBlob' has at least 'length'
    // bytes starting at 'place', and 'dstBuffer' has room for 'length' bytes.
{
    BSLS_ASSERT(place.first < srcBlob.numBuffers());
    BSLS_ASSERT(place.second < srcBlob.buffer(place.first).size());
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 != dstBuffer);
    BSLS_ASSERT(length <= srcBlob.totalSize());
    // Verifying place + length is in bounds would be messy. Callers do it.

    int copied = 0;
    do {
        const bdlbb::BlobBuffer& buf = srcBlob.buffer(place.first);
        int toCopy = bsl::min(length - copied, buf.size() - place.second);
        bsl::memcpy(dstBuffer + copied, buf.data() + place.second, toCopy);
        copied += toCopy;
        ++place.first;
        place.second = 0;
    } while (copied < length);
}

}  // close unnamed namespace

namespace bdlbb {

                              // ---------------
                              // struct BlobUtil
                              // ---------------

// CLASS METHODS
void BlobUtil::append(Blob *dest, const Blob& source, int offset, int length)
{
    BSLS_ASSERT(0 != dest);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(offset <= source.length());
    BSLS_ASSERT(length <= source.length() - offset);

    if (0 == length) {
        return;                                                       // RETURN
    }

    bsl::pair<int, int> beginPlace = findBufferIndexAndOffset(source, offset);
    int                 sourceBufferIndex  = beginPlace.first;
    int                 offsetInThisBuffer = beginPlace.second;

    const int destStartLength = dest->length();

    dest->trimLastDataBuffer();
    dest->removeUnusedBuffers();

    // Calculate how much room will be need in the buffer vector in 'dest' to
    // accomodate the new buffers that will be appended.

    {
        const int endPlaceOffset = bsl::min(source.length() - 1,
                                            offset + length);
        bsl::pair<int, int> endPlace =
                     bdlbb::BlobUtil::findBufferIndexAndOffset(source,
                                                               endPlaceOffset);
        dest->reserveBufferCapacity(dest->numDataBuffers() +
                                     (endPlace.first - sourceBufferIndex) + 1);
    }

    // Add aliased source buffer.

    int numBytesRemaining = length;

    {
        BlobBuffer src = source.buffer(sourceBufferIndex);

        if (0 < offsetInThisBuffer) {
            src.buffer().loadAlias(src.buffer(),
                                   src.data() + offsetInThisBuffer);
            src.setSize(src.size() - offsetInThisBuffer);
        }

        if (src.size() > numBytesRemaining) {
            src.setSize(numBytesRemaining);
        }

        dest->appendDataBuffer(src);

        ++sourceBufferIndex;
        numBytesRemaining -= src.size();
    }

    // Add remaining buffers.

    while (0 < numBytesRemaining) {
        BSLS_ASSERT(sourceBufferIndex < source.numBuffers());

        BlobBuffer src = source.buffer(sourceBufferIndex);

        if (src.size() > numBytesRemaining) {
            src.setSize(numBytesRemaining);
        }

        dest->appendDataBuffer(src);

        ++sourceBufferIndex;
        numBytesRemaining -= src.size();
    }

    // Set new length.

    int newLength = destStartLength + length;

    BSLS_ASSERT(-numBytesRemaining == dest->totalSize() - newLength);
    BSLS_ASSERT(newLength <= dest->totalSize());

    (void)newLength;  // quash potential compiler warning
}

void BlobUtil::append(Blob *dest, const char *source, int offset, int length)
{
    BSLS_ASSERT(0 != dest);
    BSLS_ASSERT(0 != source || 0 == length);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);

    int destBufferIndex = bsl::max(0, dest->numDataBuffers() - 1);
    int writePosition   = dest->lastDataBufferLength();

    dest->setLength(dest->length() + length);

    int numBytesLeft   = length;
    int numBytesCopied = 0;

    while (0 < numBytesLeft) {
        const BlobBuffer& buffer = dest->buffer(destBufferIndex);

        int numBytesAvailable = buffer.size() - writePosition;
        int numBytesToCopy    = bsl::min(numBytesAvailable, numBytesLeft);

        if (0 == numBytesToCopy) {
            writePosition = 0;
            ++destBufferIndex;
            continue;
        }

        BSLS_ASSERT(0 <= numBytesToCopy);

        bsl::memcpy(buffer.data() + writePosition,
                    source + offset + numBytesCopied,
                    numBytesToCopy);

        numBytesCopied += numBytesToCopy;
        numBytesLeft -= numBytesToCopy;

        if (0 < numBytesLeft) {
            ++destBufferIndex;
            writePosition = 0;
        }
    }
}

void BlobUtil::appendWithCapacityBuffer(Blob       *dest,
                                        BlobBuffer *buffer,
                                        const char *source,
                                        int         length)
{
    BSLS_ASSERT(dest);
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(buffer);

    if (dest->totalSize() - dest->length() >= length) {
        // The blob has enough capacity
        append(dest, source, length);
    }
    else {
        if (buffer->size() > 0) {
            // Append the capacity buffer
            dest->appendBuffer(bslmf::MovableRefUtil::move(*buffer));
        }
        append(dest, source, length);
        *buffer = dest->trimLastDataBuffer();
    }
}

void BlobUtil::erase(Blob *blob, int offset, int length)
{
    BSLS_ASSERT(0 != blob);
    BSLS_ASSERT(offset >= 0);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(offset <= blob->length());
    BSLS_ASSERT(length <= blob->length() - offset);

    if (0 == length) {
        return;                                                       // RETURN
    }

    bsl::pair<int, int> place = findBufferIndexAndOffset(*blob, offset);
    int                 currBufferIdx    = place.first;
    int                 leadingBufferLen = place.second;

    if (currBufferIdx >= blob->numDataBuffers()) {
        return;                                                       // RETURN
    }

    if (leadingBufferLen) {
        const BlobBuffer&     leadingBuffer = blob->buffer(currBufferIdx);
        bsl::shared_ptr<char> leadingShptr(leadingBuffer.buffer(),
                                           leadingBuffer.data());
        BlobBuffer            leadingPartialBuffer;

        leadingPartialBuffer.setSize(leadingBufferLen);

        blob->insertBuffer(currBufferIdx, leadingPartialBuffer);
        leadingPartialBuffer.buffer().swap(leadingShptr);
        blob->swapBufferRaw(currBufferIdx, &leadingPartialBuffer);

        // Adjust the length of the data to be deleted to include the length of
        // the leading buffer that we just prepended and increment
        // currBufferIdx to show that the next buffer to be deleted should be
        // after the prepended buffer.

        ++currBufferIdx;
        length += leadingBufferLen;
    }

    int nextBufferIdx = currBufferIdx;

    while (length > 0) {
        const int nextBufferSize = blob->buffer(nextBufferIdx).size();

        if (nextBufferSize <= length) {
            ++nextBufferIdx;
            length -= nextBufferSize;
        }
        else {
            break;
        }
    }

    blob->removeBuffers(currBufferIdx, nextBufferIdx - currBufferIdx);

    if (length > 0) {
        const BlobBuffer& currBlobBuffer = blob->buffer(currBufferIdx);
        const int         currBufferSize = currBlobBuffer.size();

        int       numBytesToAdjust = 0;
        const int lastDataBufLen   = blob->lastDataBufferLength();
        if (currBufferIdx == blob->numDataBuffers() - 1 &&
            lastDataBufLen < currBufferSize) {
            numBytesToAdjust = currBufferSize - lastDataBufLen;
        }

        bsl::shared_ptr<char> trailingShptr(currBlobBuffer.buffer(),
                                            currBlobBuffer.data() + length);

        BlobBuffer trailingPartialBuffer;
        trailingPartialBuffer.setSize(currBufferSize - length);

        blob->insertBuffer(currBufferIdx, trailingPartialBuffer);
        trailingPartialBuffer.buffer().swap(trailingShptr);
        blob->swapBufferRaw(currBufferIdx, &trailingPartialBuffer);
        blob->removeBuffer(currBufferIdx + 1);
        if (numBytesToAdjust) {
            blob->setLength(blob->length() - numBytesToAdjust);
        }
    }
}

void BlobUtil::insert(Blob        *dest,
                      int          destOffset,
                      const Blob&  source,
                      int          sourceOffset,
                      int          sourceLength)
{
    BSLS_ASSERT(0 != dest);

    // TBD: optimize this also

    Blob result;

    append(&result, *dest, 0, destOffset);
    append(&result, source, sourceOffset, sourceLength);
    append(&result, *dest, destOffset);

    *dest = result;
}

bsl::pair<int, int> BlobUtil::findBufferIndexAndOffset(const Blob& blob,
                                                       int         position)
{
    BSLS_ASSERT(0 <= position);
    BSLS_ASSERT(position < blob.totalSize());

    bsl::pair<int, int>  result(0, position);
    const BlobBuffer    *buffer = &(blob.buffer(0));
    for (; buffer->size() <= result.second; ++buffer) {
        result.first++;
        result.second -= buffer->size();
        BSLS_ASSERT(result.first < blob.numBuffers());
    }
    return result;
}

void BlobUtil::copy(char        *dstBuffer,
                    const Blob&  srcBlob,
                    int          position,
                    int          length)
{
    BSLS_ASSERT(0 <= position);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(position <= srcBlob.totalSize() - length);
    BSLS_ASSERT(dstBuffer != 0);

    if (0 < length) {
        copyFromPlace(dstBuffer,
                      srcBlob,
                      findBufferIndexAndOffset(srcBlob, position),
                      length);
    }
}

void BlobUtil::copy(Blob *dst, int dstOffset, const char *src, int length)
{
    BSLS_ASSERT(0 <= dstOffset);
    BSLS_ASSERT(0 <= length);

    if (0 != length) {
        BSLS_ASSERT(dst);
        BSLS_ASSERT(src);
        BSLS_ASSERT(dstOffset <= dst->length() - length);

        bsl::pair<int, int> place = findBufferIndexAndOffset(*dst, dstOffset);

        int copied    = 0;
        int bufIdx    = place.first;
        int bufOffset = place.second;

        do {
            const BlobBuffer& buf = dst->buffer(bufIdx);
            int toCopy = bsl::min(length - copied, buf.size() - bufOffset);
            bsl::memcpy(buf.data() + bufOffset, src + copied, toCopy);
            copied += toCopy;
            ++bufIdx;
            bufOffset = 0;
        } while (copied < length);
    }
}

void BlobUtil::copy(Blob        *dst,
                    int          dstOffset,
                    const Blob&  src,
                    int          srcOffset,
                    int          length)
{
    BSLS_ASSERT(0 <= dstOffset);
    BSLS_ASSERT(0 <= srcOffset);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(srcOffset <= src.length() - length);

    if (0 != length) {
        BSLS_ASSERT(dst);
        BSLS_ASSERT(dstOffset <= dst->length() - length);

        bsl::pair<int, int> dstPlace =
            findBufferIndexAndOffset(*dst, dstOffset);
        bsl::pair<int, int> srcPlace =
            findBufferIndexAndOffset(src, srcOffset);

        int copied       = 0;
        int dstBufIdx    = dstPlace.first;
        int dstBufOffset = dstPlace.second;
        int srcBufIdx    = srcPlace.first;
        int srcBufOffset = srcPlace.second;

        do {
            const BlobBuffer& dstBuf = dst->buffer(dstBufIdx);
            const BlobBuffer& srcBuf = src.buffer(srcBufIdx);

            int toCopy = bsl::min(
                bsl::min(length - copied, dstBuf.size() - dstBufOffset),
                srcBuf.size() - srcBufOffset);

            bsl::memcpy(dstBuf.data() + dstBufOffset,
                        srcBuf.data() + srcBufOffset,
                        toCopy);

            copied += toCopy;

            if (toCopy == dstBuf.size() - dstBufOffset) {
                ++dstBufIdx;
                dstBufOffset = 0;
            }
            else {
                dstBufOffset += toCopy;
            }

            if (toCopy == srcBuf.size() - srcBufOffset) {
                ++srcBufIdx;
                srcBufOffset = 0;
            }
            else {
                srcBufOffset += toCopy;
            }
        } while (copied < length);
    }
}

char *BlobUtil::getContiguousRangeOrCopy(char        *dstBuffer,
                                         const Blob&  srcBlob,
                                         int          position,
                                         int          length,
                                         int          alignment)
{
    BSLS_ASSERT(dstBuffer != 0);
    BSLS_ASSERT(0 <= position);
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(length <= srcBlob.totalSize());
    BSLS_ASSERT(position <= srcBlob.totalSize() - length);
    BSLS_ASSERT(0 < alignment);
    BSLS_ASSERT(0 == (alignment & (alignment - 1)));
    BSLS_ASSERT(0 == (reinterpret_cast<bsls::Types::IntPtr>(dstBuffer) &
                      (alignment - 1)));

    bsl::pair<int, int>  place  = findBufferIndexAndOffset(srcBlob, position);
    const BlobBuffer&    buffer = srcBlob.buffer(place.first);
    char                *p      = buffer.data() + place.second;
    if (0 != (reinterpret_cast<bsls::Types::IntPtr>(p) & (alignment - 1)) ||
        buffer.size() - place.second < length) {
        copyFromPlace(dstBuffer, srcBlob, place, length);
        p = dstBuffer;
    }
    return p;
}

char *BlobUtil::getContiguousDataBuffer(Blob              *blob,
                                        int                addLength,
                                        BlobBufferFactory *factory)
{
    BSLS_ASSERT(factory != 0);
    BSLS_ASSERT(blob != 0);
    BSLS_ASSERT(0 < addLength);

    int index  = blob->numDataBuffers() - 1;
    int offset = blob->lastDataBufferLength();
    if (index < 0 || blob->buffer(index).size() - offset < addLength) {
        blob->trimLastDataBuffer();
        int size = offset = 0;
        while (++index < blob->numBuffers() &&
               0 == (size = blob->buffer(index).size())) {
            ;  // skip past any zero-size buffers
        }
        if (size < addLength) {
            index = blob->numDataBuffers();  // found nothing usable
            BlobBuffer buffer;
            factory->allocate(&buffer);
            BSLS_ASSERT(addLength <= buffer.size());
            blob->insertBuffer(index, buffer);
        }
    }
    blob->setLength(blob->length() + addLength);
    return blob->buffer(index).data() + offset;
}

bsl::ostream& BlobUtil::asciiDump(bsl::ostream& stream, const Blob& source)
{
    int numBytes = source.length();

    for (int numBytesRemaining = numBytes, i = 0; 0 < numBytesRemaining; ++i) {
        BSLS_ASSERT(i < source.numBuffers());

        const BlobBuffer& buffer = source.buffer(i);

        int bytesToWrite = numBytesRemaining < buffer.size()
                               ? numBytesRemaining
                               : buffer.size();

        stream.write(buffer.data(), bytesToWrite);
        numBytesRemaining -= bytesToWrite;
    }

    return stream;
}

bsl::ostream& BlobUtil::hexDump(bsl::ostream& stream,
                                const Blob&   source,
                                int           offset,
                                int           length)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(length <= source.length());
    BSLS_ASSERT(offset <= source.length() - length);

    if (0 == source.length() || 0 == length) {
        return stream;                                                // RETURN
    }

    bsl::pair<int, int> place       = findBufferIndexAndOffset(source, offset);
    int                 bufferIndex = place.first;
    int                 offsetInThisBuffer = place.second;

    if (bufferIndex >= source.numDataBuffers()) {
        return stream;                                                // RETURN
    }

    enum { k_NUM_STATIC_BUFFERS = 32 };

    typedef bsl::pair<const char *, int> BufferInfo;

    BufferInfo  staticBuffers[k_NUM_STATIC_BUFFERS];
    BufferInfo *buffers       = staticBuffers;
    int         numBufferInfo = 0;

    bslma::DeallocatorProctor<bslma::Allocator> deallocationGuard(
        0, bslma::Default::defaultAllocator());

    const int NUM_BUFFERS = source.numDataBuffers() - bufferIndex;
    if (NUM_BUFFERS > k_NUM_STATIC_BUFFERS) {
        // This works because we do not need to call the constructor on a pair
        // of two built-in types.

        buffers = static_cast<BufferInfo *>(
                      bslma::Default::defaultAllocator()->allocate(
                                            NUM_BUFFERS * sizeof(BufferInfo)));
        deallocationGuard.reset(buffers);
    }

    int numBytesLeft   = length;
    int numBytesCopied = 0;

    while (0 < numBytesLeft) {
        BSLS_ASSERT(bufferIndex < source.numDataBuffers());

        const BlobBuffer& buffer = source.buffer(bufferIndex);

        int startingIndex = 0 == numBytesCopied ? offsetInThisBuffer : 0;

        int numBytesAvailable = bufferIndex == source.numDataBuffers() - 1
                                    ? source.lastDataBufferLength()
                                    : buffer.size() - startingIndex;

        int numBytesToDump = bsl::min(numBytesAvailable, numBytesLeft);

        if (0 == numBytesToDump) {
            ++bufferIndex;
            continue;
        }

        BSLS_ASSERT(0 <= numBytesToDump);

        buffers[numBufferInfo] =
            bsl::make_pair(buffer.data() + startingIndex, numBytesToDump);
        ++numBufferInfo;

        numBytesCopied += numBytesToDump;
        numBytesLeft -= numBytesToDump;

        if (0 < numBytesLeft) {
            ++bufferIndex;
        }
    }

    return bdlb::Print::hexDump(stream, buffers, numBufferInfo);
}

int BlobUtil::compare(const Blob& a, const Blob& b)
{
    // Upon entry, establish 'lhs' and 'rhs' as aliases for 'a' and 'b',
    // respectively.  The logic of this function is such that the blob with the
    // smaller current blob buffer is on the left-hand side of comparison
    // operations.  Consequently, using 'lhs' and 'rhs' in place of 'a' and 'b'
    // enhances the clarity of the algorithm.

    const Blob& lhs = a;
    const Blob& rhs = b;

    const int lhsLen = lhs.length();
    const int rhsLen = rhs.length();
    const int minLen = bsl::min(lhsLen, rhsLen);

    if (0 == minLen) {
        return lhsLen - rhsLen;                                       // RETURN
    }

    const BlobBuffer& lhsBlobBuffer = lhs.buffer(0);
    const BlobBuffer& rhsBlobBuffer = rhs.buffer(0);

    // Keep track of the size of the blob buffers and store the smaller blob
    // buffer in the 'lhs' blob buffer.

    bool isLhsBufSmaller = lhsBlobBuffer.size() < rhsBlobBuffer.size();

    int lhsBufSize;
    int rhsBufSize;

    const char *lhsPtr;
    const char *rhsPtr;

    const Blob *lhsBlob;
    const Blob *rhsBlob;

    if (isLhsBufSmaller) {
        lhsBufSize = lhsBlobBuffer.size();
        lhsPtr     = lhsBlobBuffer.data();
        lhsBlob    = &lhs;

        rhsBufSize = rhsBlobBuffer.size();
        rhsPtr     = rhsBlobBuffer.data();
        rhsBlob    = &rhs;
    }
    else {
        lhsBufSize = rhsBlobBuffer.size();
        lhsPtr     = rhsBlobBuffer.data();
        lhsBlob    = &rhs;

        rhsBufSize = lhsBlobBuffer.size();
        rhsPtr     = lhsBlobBuffer.data();
        rhsBlob    = &lhs;
    }

    // Keep track of which buffer we are comparing.

    int lhsBufIdx = 0;
    int rhsBufIdx = 0;

    int numBytesRemaining = minLen;

    while (numBytesRemaining > 0) {
        const int numBytesToCompare = bsl::min(lhsBufSize, numBytesRemaining);

        // Note this code can tolerate the case where '0 == lhsBufSize'.  We
        // just DROP through and increment to the next buffer.  Note that when
        // the two buffers are the same size, an iteration is made where
        // '0 == lhsBufSize' holds because we only advance one of the two
        // buffers in each iteration of the loop.

        const int rc = bsl::memcmp(static_cast<const void *>(lhsPtr),
                                   static_cast<const void *>(rhsPtr),
                                   numBytesToCompare);

        if (rc) {
            return isLhsBufSmaller ? rc : -rc;                        // RETURN
        }

        numBytesRemaining -= numBytesToCompare;

        if (0 == numBytesRemaining) {
            break;
        }

        rhsBufSize -= numBytesToCompare;

        ++lhsBufIdx;  // advance to next 'lhs' blob buffer
        BSLS_ASSERT(lhsBufIdx < lhsBlob->numDataBuffers());

        const BlobBuffer& nextLhsBlobBuffer = lhsBlob->buffer(lhsBufIdx);
        const int         nextLhsBufSize    = nextLhsBlobBuffer.size();
        if (rhsBufSize >= nextLhsBufSize) {
            // The next 'lhs' blob buffer is still the smaller buffer.

            lhsBufSize = nextLhsBufSize;
            lhsPtr     = nextLhsBlobBuffer.data();

            rhsPtr = rhsPtr + numBytesToCompare;
        }
        else {
            // The new 'lhs' blob buffer is larger than the 'rhs' blob buffer,
            // so swap them so that the smaller buffer is on the left.

            isLhsBufSmaller = !isLhsBufSmaller;

            lhsBufSize = rhsBufSize;
            lhsPtr     = rhsPtr + numBytesToCompare;

            rhsBufSize = nextLhsBufSize;
            rhsPtr     = nextLhsBlobBuffer.data();

            using bsl::swap;
            swap(lhsBlob, rhsBlob);
            swap(lhsBufIdx, rhsBufIdx);
        }
    }

    // Everything is the same, only the lengths may differ.

    return lhsLen - rhsLen;
}

void BlobUtil::prependWithCapacityBuffer(Blob       *dest,
                                         BlobBuffer *buffer,
                                         const char *source,
                                         int         length)
{
    BSLS_ASSERT(dest);
    BSLS_ASSERT(source);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(buffer);

    if (0 == length) {
        return;                                                       // RETURN
    }

    if (0 == dest->length()) {
        appendWithCapacityBuffer(dest, buffer, source, length);
        return;                                                       // RETURN
    }

    BlobBuffer nextBuffer;
    if (0 == buffer->size()) {
        dest->factory()->allocate(&nextBuffer);
    }
    else {
        nextBuffer = bslmf::MovableRefUtil::move(*buffer);
    }

    int blobBufferIndex = 0;
    while (nextBuffer.size() < length) {
        int size = nextBuffer.size();
        bsl::memcpy(nextBuffer.data(), source, size);
        dest->insertBuffer(blobBufferIndex++,
                           bslmf::MovableRefUtil::move(nextBuffer));
        length -= size;
        source += size;

        dest->factory()->allocate(&nextBuffer);
    }

    bsl::memcpy(nextBuffer.data(), source, length);
    *buffer = nextBuffer.trim(length);
    dest->insertBuffer(blobBufferIndex,
                       bslmf::MovableRefUtil::move(nextBuffer));
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
