// bcema_blobutil.cpp                                                 -*-C++-*-
#include <bcema_blobutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_blobutil_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bdeu_print.h>

#include <bsl_algorithm.h>

#include <bsl_c_ctype.h>
#include <bsl_iostream.h>

namespace BloombergLP {
namespace {

// HELPER FUNCTIONS
void findBufferIndexFromOffset(int               *bufferIndex,
                               int               *previousBuffersLength,
                               const bcema_Blob&  blob,
                               int                offset)
{
    BSLS_ASSERT(bufferIndex);
    BSLS_ASSERT(previousBuffersLength);
    BSLS_ASSERT(offset <= blob.length());

    int i = 0;

    for (*previousBuffersLength = 0; i < blob.numBuffers(); ++i) {
        bcema_BlobBuffer buffer = blob.buffer(i);

        if (*previousBuffersLength + buffer.size() > offset) {
            break;
        }

        *previousBuffersLength += buffer.size();
    }

    *bufferIndex = i;
}

void findFirstAvailableBuffer(int               *firstAvailableBuffer,
                              int               *previousBuffersLength,
                              const bcema_Blob&  blob)
    // Return the index of the first buffer with unused bytes, or
    // blob.numBuffers() if all buffers are completely used.
{
    return findBufferIndexFromOffset(firstAvailableBuffer,
                                     previousBuffersLength,
                                     blob,
                                     blob.length());
}

}  // close unnamed namespace

                           // ---------------------
                           // struct bcema_BlobUtil
                           // ---------------------

// CLASS METHODS
void bcema_BlobUtil::append(bcema_Blob        *dest,
                            const bcema_Blob&  source,
                            int                offset,
                            int                length)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(offset + length <= source.length());

    if (0 == length) {
        return;
    }

    int sourceBufferIndex;
    int sourcePreviousBuffersLength;

    findBufferIndexFromOffset(&sourceBufferIndex,
                              &sourcePreviousBuffersLength,
                              source,
                              offset);

    BSLS_ASSERT(sourceBufferIndex < source.numBuffers());

    int destBufferIndex;
    int destPreviousBuffersLength;

    findFirstAvailableBuffer(&destBufferIndex,
                             &destPreviousBuffersLength,
                             *dest);

    int destStartLength = dest->length();

    // Trim destination blob.

    if (dest->totalSize() != destStartLength) {
        BSLS_ASSERT(destBufferIndex < dest->numBuffers());

        bcema_BlobBuffer lastBuffer        = dest->buffer(destBufferIndex);
        int              newLastBufferSize = dest->length()
                                                   - destPreviousBuffersLength;

        BSLS_ASSERT(lastBuffer.size() > newLastBufferSize);

        while (destBufferIndex != dest->numBuffers()) {
            dest->removeBuffer(destBufferIndex);
        }

        if (0 != newLastBufferSize) {
            lastBuffer.setSize(newLastBufferSize);
            dest->appendBuffer(lastBuffer);
        }

        BSLS_ASSERT(dest->totalSize() == destStartLength);
    }

    // Add aliased source buffer.

    int numBytesRemaining = length;

    {
        bcema_BlobBuffer src = source.buffer(sourceBufferIndex);

        if (offset > sourcePreviousBuffersLength) {
            int offsetInThisBuffer = offset - sourcePreviousBuffersLength;

            src.buffer().loadAlias(src.buffer(),
                                   src.data() + offsetInThisBuffer);

            src.setSize(src.size() - offsetInThisBuffer);
        }

        if (src.size() > numBytesRemaining) {
            src.setSize(numBytesRemaining);
        }

        dest->appendBuffer(src);

        ++sourceBufferIndex;
        numBytesRemaining -= src.size();
    }

    // Add remaining buffers.

    while (0 < numBytesRemaining) {
        BSLS_ASSERT(sourceBufferIndex < source.numBuffers());

        bcema_BlobBuffer src = source.buffer(sourceBufferIndex);

        if (src.size() > numBytesRemaining) {
            src.setSize(numBytesRemaining);
        }

        dest->appendBuffer(src);

        ++sourceBufferIndex;
        numBytesRemaining -= src.size();
    }

    // Set new length.

    int newLength = destStartLength + length;

    BSLS_ASSERT(-numBytesRemaining == dest->totalSize() - newLength);
    BSLS_ASSERT(newLength          <= dest->totalSize());

    dest->setLength(newLength);
}

void bcema_BlobUtil::append(bcema_Blob *dest,
                            const char *source,
                            int         offset,
                            int         length)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);

    int destBufferIndex = bsl::max(0, dest->numDataBuffers() - 1);
    int writePosition   = dest->lastDataBufferLength();

    dest->setLength(dest->length() + length);

    int numBytesLeft   = length;
    int numBytesCopied = 0;

    while (0 < numBytesLeft) {
        const bcema_BlobBuffer& buffer = dest->buffer(destBufferIndex);

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
        numBytesLeft   -= numBytesToCopy;

        if (0 < numBytesLeft) {
            ++destBufferIndex;
            writePosition = 0;
        }
    }
}

void bcema_BlobUtil::erase(bcema_Blob *blob, int offset, int length)
{
    BSLS_ASSERT(blob);
    BSLS_ASSERT(offset >= 0);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(offset + length <= blob->length());

    if (0 == length) {
        return;                                                       // RETURN
    }

    int currBufferIdx;
    int prevBuffersLen;
    findBufferIndexFromOffset(&currBufferIdx, &prevBuffersLen, *blob, offset);

    if (currBufferIdx >= blob->numDataBuffers()) {
        return;                                                       // RETURN
    }

    int leadingBufferLen = offset - prevBuffersLen;
    if (leadingBufferLen) {
        const bcema_BlobBuffer& leadingBuffer = blob->buffer(currBufferIdx);
        bcema_SharedPtr<char>   leadingShptr(leadingBuffer.buffer(),
                                             leadingBuffer.data());
        bcema_BlobBuffer        leadingPartialBuffer;

        leadingPartialBuffer.setSize(leadingBufferLen);

        blob->insertBuffer(currBufferIdx, leadingPartialBuffer);
        leadingPartialBuffer.buffer().swap(leadingShptr);
        blob->swapBufferRaw(currBufferIdx, &leadingPartialBuffer);

        // Adjust the length of the data to be deleted to include the length
        // of the leading buffer that we just prepended and increment
        // currBufferIdx to show that the next buffer to be deleted should be
        // after the prepended buffer.
        ++currBufferIdx;
        length += leadingBufferLen;
    }

    while (length > 0) {
        const bcema_BlobBuffer& currBlobBuffer = blob->buffer(currBufferIdx);
        int                     currBufferSize = currBlobBuffer.size();
        if (currBufferSize <= length) {
            blob->removeBuffer(currBufferIdx);
            length -= currBufferSize;
        }
        else {
            int       numBytesToAdjust = 0;
            const int lastDataBufLen   = blob->lastDataBufferLength();
            if (currBufferIdx == blob->numDataBuffers() - 1
             && lastDataBufLen < currBufferSize) {
                numBytesToAdjust = currBufferSize - lastDataBufLen;
            }

            const int trailingBufferLen = currBufferSize - length;
            bcema_SharedPtr<char> trailingShptr(
                                               currBlobBuffer.buffer(),
                                               currBlobBuffer.data() + length);
            bcema_BlobBuffer      trailingPartialBuffer;
            trailingPartialBuffer.setSize(trailingBufferLen);

            blob->insertBuffer(currBufferIdx, trailingPartialBuffer);
            trailingPartialBuffer.buffer().swap(trailingShptr);
            blob->swapBufferRaw(currBufferIdx, &trailingPartialBuffer);
            blob->removeBuffer(currBufferIdx + 1);
            if (numBytesToAdjust) {
                blob->setLength(blob->length() - numBytesToAdjust);
            }
            break;
        }
    }
}

void bcema_BlobUtil::insert(bcema_Blob        *dest,
                            int                destOffset,
                            const bcema_Blob&  source,
                            int                sourceOffset,
                            int                sourceLength)
{
    // TBD: optimize this also

    bcema_Blob result;

    append(&result, *dest, 0, destOffset);
    append(&result, source, sourceOffset, sourceLength);
    append(&result, *dest, destOffset);

    *dest = result;
}

bsl::ostream& bcema_BlobUtil::asciiDump(bsl::ostream&     stream,
                                        const bcema_Blob& source)
{
    int numBytes = source.length();

    for (int numBytesRemaining = numBytes, i = 0; 0 < numBytesRemaining; ++i) {
        BSLS_ASSERT(i < source.numBuffers());

        const bcema_BlobBuffer& buffer = source.buffer(i);

        int bytesToWrite = numBytesRemaining < buffer.size()
                           ? numBytesRemaining
                           : buffer.size();

        stream.write(buffer.data(), bytesToWrite);
        numBytesRemaining -= bytesToWrite;
    }

    return stream;
}

bsl::ostream& bcema_BlobUtil::hexDump(bsl::ostream&     stream,
                                      const bcema_Blob& source,
                                      int               offset,
                                      int               length)
{
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(0 <= length);

    if (0 == source.numDataBuffers()) {
        return stream;
    }

    enum {
        NUM_STATIC_BUFFERS = 32
    };

    typedef bsl::pair<const char*, int> BufferInfo;

    BufferInfo  staticBuffers[NUM_STATIC_BUFFERS];
    BufferInfo *buffers = staticBuffers;
    int         numBufferInfo = 0;

    bslma_Allocator *allocator = bslma_Default::defaultAllocator();
    bslma_DeallocatorProctor<bslma_Allocator> deallocationGuard(0, allocator);

    if (source.numDataBuffers() > NUM_STATIC_BUFFERS) {
        // This works because we do not need to call the constructor on a pair
        // of two built-in types.

        buffers = (BufferInfo*) allocator->allocate(source.numDataBuffers() *
                                                    sizeof (BufferInfo));
        deallocationGuard.reset(buffers);
    }

    int bufferIndex;
    int previousBuffersLength;

    findBufferIndexFromOffset(&bufferIndex,
                              &previousBuffersLength,
                              source,
                              offset);

    if (bufferIndex >= source.numDataBuffers()) {
        return stream;
    }

    BSLS_ASSERT(bufferIndex < source.numDataBuffers());

    int numBytesLeft   = length;
    int numBytesCopied = 0;

    while (0 < numBytesLeft) {
        BSLS_ASSERT(bufferIndex < source.numDataBuffers());

        const bcema_BlobBuffer& buffer = source.buffer(bufferIndex);

        int startingIndex     = 0 == numBytesCopied
                                ? offset - previousBuffersLength
                                : 0;

        int numBytesAvailable = bufferIndex == source.numDataBuffers() - 1
                                ? source.lastDataBufferLength()
                                : buffer.size() - startingIndex;

        int numBytesToDump    = bsl::min(numBytesAvailable, numBytesLeft);

        if (0 == numBytesToDump) {
            ++bufferIndex;
            continue;
        }

        BSLS_ASSERT(0 <= numBytesToDump);

        buffers[numBufferInfo] = bsl::make_pair(buffer.data() + startingIndex,
                                                numBytesToDump);
        ++numBufferInfo;

        numBytesCopied += numBytesToDump;
        numBytesLeft   -= numBytesToDump;

        if (0 < numBytesLeft) {
            ++bufferIndex;
        }
    }

    return bdeu_Print::hexDump(stream, buffers, numBufferInfo);
}

int bcema_BlobUtil::compare(const bcema_Blob& a, const bcema_Blob& b)
{
    // Upon entry, establish 'lhs' and 'rhs' as aliases for 'a' and 'b',
    // respectively.  The logic of this function is such that the blob with the
    // smaller current blob buffer is on the left-hand side of comparison
    // operations.  Consequently, using 'lhs' and 'rhs' in place of 'a' and 'b'
    // enhances the clarity of the algorithm.

    const bcema_Blob& lhs = a;
    const bcema_Blob& rhs = b;

    const int lhsLen = lhs.length();
    const int rhsLen = rhs.length();
    const int minLen = bsl::min(lhsLen, rhsLen);

    if (0 == minLen) {
        return lhsLen - rhsLen;                                       // RETURN
    }

    const bcema_BlobBuffer& lhsBlobBuffer = lhs.buffer(0);
    const bcema_BlobBuffer& rhsBlobBuffer = rhs.buffer(0);

    // Keep track of the size of the blob buffers and store the smaller blob
    // buffer in the 'lhs' blob buffer.

    bool isLhsBufSmaller = lhsBlobBuffer.size() < rhsBlobBuffer.size();

    int lhsBufSize;
    int rhsBufSize;

    const char *lhsPtr;
    const char *rhsPtr;

    const bcema_Blob *lhsBlob;
    const bcema_Blob *rhsBlob;

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
            return isLhsBufSmaller ? rc : -rc;
        }

        numBytesRemaining -= numBytesToCompare;

        if (0 == numBytesRemaining) {
            break;
        }

        rhsBufSize -= numBytesToCompare;

        ++lhsBufIdx;  // advance to next 'lhs' blob buffer
        BSLS_ASSERT(lhsBufIdx < lhsBlob->numDataBuffers());

        const bcema_BlobBuffer& nextLhsBlobBuffer = lhsBlob->buffer(lhsBufIdx);
        const int               nextLhsBufSize    = nextLhsBlobBuffer.size();
        if (rhsBufSize >= nextLhsBufSize) {
            // The next 'lhs' blob buffer is still the smaller buffer.

            lhsBufSize = nextLhsBufSize;
            lhsPtr     = nextLhsBlobBuffer.data();

            rhsPtr     = rhsPtr + numBytesToCompare;
        } else {
            // The new 'lhs' blob buffer is larger than the 'rhs' blob buffer,
            // so swap them so that the smaller buffer is on the left.

            isLhsBufSmaller = !isLhsBufSmaller;

            lhsBufSize = rhsBufSize;
            lhsPtr     = rhsPtr + numBytesToCompare;

            rhsBufSize = nextLhsBufSize;
            rhsPtr     = nextLhsBlobBuffer.data();

            bsl::swap(lhsBlob,   rhsBlob);
            bsl::swap(lhsBufIdx, rhsBufIdx);
        }
    }

    // Everything is the same, only the lengths may differ.

    return lhsLen - rhsLen;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
