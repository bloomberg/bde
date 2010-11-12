// bcema_blobutil.cpp              -*-C++-*-
#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")



#include <bcema_blobutil.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bdeu_print.h>

#include <ctype.h>
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

        std::memcpy(buffer.data() + writePosition,
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

        int bytesToWrite = (numBytesRemaining < buffer.size())
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

int bcema_BlobUtil::compare(const bcema_Blob& b1, const bcema_Blob& b2)
{
    int b1l = b1.length();
    int b2l = b2.length();
    int bl = bsl::min(b2l, b1l);


    // Initialize the loop with the correct indices and pointers.
    // This can use some optimization.

    // We need to know which of the first buffers are larger
    int b1bl = b1.buffer(0).size();
    int b2bl = b2.buffer(0).size();

    bool isB1Smaller = b1bl < b2bl;

    // Keep track of the size
    int ssz = isB1Smaller ? b1bl : b2bl;
    int bsz = isB1Smaller ? b2bl : b1bl;


    // Keep track of the pointers to the data
    const char *sptr = isB1Smaller ? b1.buffer(0).data()
                                   : b2.buffer(0).data();
    const char *bptr = isB1Smaller ? b2.buffer(0).data()
                                   : b1.buffer(0).data();

    // Keep track of which blobs
    const bcema_Blob *sb = isB1Smaller ? &b1 : &b2;
    const bcema_Blob *bb = isB1Smaller ? &b2 : &b1;

    // Keep track of what buffer we're on.
    int si = 0;
    int bi = 0;

    // This will do an extra iteration when the 2 sections being compared are
    // the same length.  This was a quick implementation that should be
    // optimized.
    for (int i = 0; i < bl; ) {
        // Only compare to the end of the blob length
        if (ssz > bl - i)
           ssz = bl - i;

        int rc = std::memcmp((const void *)sptr, (const void *)bptr, ssz);
        if (rc != 0) {
            return isB1Smaller ? rc : -rc;
        }
        i += ssz;
        bsz -= ssz;
        ++si;
        int newsz = sb->buffer(si).size();
        if (bsz >= newsz) {     // The old buffer is still bigger
            bptr = bptr + ssz;
            sptr = sb->buffer(si).data();
            ssz = newsz;
        } else {                // The new buffer will be bigger
            // Swap the small and big buffers
            isB1Smaller = !isB1Smaller;
            const bcema_Blob *tmp = sb;
            sb = bb;
            bb = tmp;

            int tmpi = si;
            si = bi;
            bi = tmpi;

            sptr = bptr + ssz;
            ssz  = bsz;
            bsz  = newsz;
            bptr = bb->buffer(bi).data();
        }
    }
    return b1l - b2l;  // everything is the same, only the lengths differ
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
