// btes_iovecutil.cpp            -*-C++-*-
#include <btes_iovecutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes_iovecutil_cpp,"$Id$ $CSID$")

#include <bcema_pooledbufferchain.h>
#include <bcema_blob.h>
#include <bcema_pooledblobbufferfactory.h>
#include <bslma_allocator.h>
#include <bsls_platformutil.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

namespace {

                         // ------------------------
                         // local function templates
                         // ------------------------

template <class IOVEC>
inline
void genericAppendToBlob(bcema_Blob  *blob,
                         const IOVEC *vecs,
                         int          numVecs,
                         int          offset)
{
    BSLS_ASSERT(offset >= 0);
    BSLS_ASSERT(numVecs > 0);


    // Set up loop invariant stated below.  Note that if blobLength is 0,
    // or if last data buffer is complete, the call 'blob->setLength(...)'
    // below will create additional buffers as needed, so that the call to
    // 'blob->buffer(currentBufIndex)' will always be legal.

    int currentBufIndex  = blob->numDataBuffers() - 1;
    int currentBufOffset = blob->lastDataBufferLength();
    if (currentBufIndex < 0 ||
        currentBufOffset == blob->buffer(currentBufIndex).size()) {
        // Blob is empty or last data buffer is complete: skip to next buffer.
        ++currentBufIndex;
        currentBufOffset = 0;
    }

    // Skip to 'offset' in 'vecs'.

    int currentVecIndex = 0;
    int currentVecAvailable = vecs[currentVecIndex].length();
    BSLS_ASSERT(0 < currentVecAvailable);

    int prefixLength = 0;
    while (prefixLength + currentVecAvailable <= offset) {
        prefixLength += currentVecAvailable;
        ++currentVecIndex;
        currentVecAvailable = vecs[currentVecIndex].length();
        BSLS_ASSERT(0 < currentVecAvailable);
    }
    int currentVecOffset = offset - prefixLength;
    currentVecAvailable -= currentVecOffset;
    BSLS_ASSERT(0 <= currentVecOffset);
    BSLS_ASSERT(0 < currentVecAvailable);

    // For simplicity,  finish computing the iovec lengths, and reserve blob's
    // length in a single setLength call.  Since prefixLength isn't used, we
    // reset it and use it for that computation.

    prefixLength = currentVecAvailable;
    for (int i = currentVecIndex+1; i < numVecs; ++i) {
        prefixLength += vecs[i].length();
    }
    blob->setLength(blob->length() + prefixLength);

    // Compute number of bytes available to read or write in current vec or
    // buffer.  This must be done *after* setting the blob length.

    int currentBufAvailable = blob->buffer(currentBufIndex).size();
    currentBufAvailable -= currentBufOffset;
    BSLS_ASSERT(0 < currentBufAvailable);

    // Append the iovecs as individual blob buffers, re-segmented to take
    // advantage of the factory's buffer size: this is a classic merge.

    while (1) {
        // Invariants:
        // 1. 0 <= currentVec < numVecs
        // 2. 0 <= currentVecOffset < vecs[currentVec].length()
        // 3. 0 <  currentVecAvailable
        // 4. 0 <= currentBuf < blobs.numDataBuffers()
        // 5. 0 <= currentBufOffset < blob->buffer(currentBuf).size()
        // 6. 0 <  currentBufAvailable

        int numBytesCopied = bsl::min(currentVecAvailable,
                                      currentBufAvailable);

        bsl::memcpy(blob->buffer(currentBufIndex).data() + currentBufOffset,
                    static_cast<const char *>(vecs[currentVecIndex].buffer())
                             + currentVecOffset,
                    numBytesCopied);

        currentBufOffset += numBytesCopied;
        currentVecOffset += numBytesCopied;

        if (currentVecAvailable == numBytesCopied) {
            currentVecOffset = 0;
            if (++currentVecIndex == numVecs) {
                return;
            }
            currentVecAvailable = vecs[currentVecIndex].length();
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
inline
bcema_PooledBufferChain *genericChain(const IOVEC                    *vecs,
                                      int                             numVecs,
                                      int                             offset,
                                      bcema_PooledBufferChainFactory *factory)
{
    BSLS_ASSERT(offset >= 0);
    BSLS_ASSERT(numVecs > 0);

    bsls_PlatformUtil::Int64 totalLength = 0;
    bcema_PooledBufferChain *chain = factory->allocate(0);
    const IOVEC *cur = &vecs[0];

    for (int i = 0; i < numVecs; ++i, ++cur) {
        bsls_PlatformUtil::Int64 newLength  = totalLength + cur->length();
        if (totalLength <= offset && offset < newLength) {
            BSLS_ASSERT(0 == chain->length());

            int offsetInMsg = offset - totalLength;
            chain->append(static_cast<char *>(cur->buffer()) + offsetInMsg,
                                              cur->length()  - offsetInMsg);
        }
        else if (totalLength > offset) {
            BSLS_ASSERT(0 < chain->length());
            chain->append(static_cast<char *>(cur->buffer()), cur->length());
        }
        totalLength = newLength;
    }

    BSLS_ASSERT(offset < totalLength);
    BSLS_ASSERT(chain->length() == (totalLength - offset));
    return chain;
}

template <class IOVEC>
inline
int genericGather(char        *buffer,
                  int          length,
                  const IOVEC *buffers,
                  int          numBuffers)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    int numCopied = 0;
    while(numBuffers-- && 0 < length) {
        if (buffers->length() < length) {
            bsl::memcpy(buffer, (char*)buffers->buffer(), buffers->length());
            numCopied += buffers->length();
            length -= buffers->length();
            buffer += buffers->length();
            ++buffers;
        }
        else {
            bsl::memcpy(buffer, (char*)buffers->buffer(), length);
            numCopied += length;
            length = 0;
        }
    }
    return numCopied;
}

template <class IOVEC>
inline
int genericLength(const IOVEC *buffers, int numBuffers)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);

    int length = 0;
    while (numBuffers--) {
        length += buffers++->length();
    }
    return length;
}

template <class IOVEC>
inline
void genericPivot(int         *bufferIdx,
                  int         *offset,
                  const IOVEC *buffers,
                  int          numBuffers,
                  int          length)
{
    BSLS_ASSERT(bufferIdx);
    BSLS_ASSERT(offset);
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(0 <= length);

    for (int idx = 0; idx < numBuffers; ++idx) {
        if (length < buffers[idx].length()) {
            *bufferIdx = idx;
            *offset = length;
            return;
        }
        length -= buffers[idx].length();
    }

    *bufferIdx = numBuffers;
    return;
}

template <class IOVEC>
inline
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
    while(numBuffers-- && 0 < length) {
        if (buffers->length() < length) {
            bsl::memcpy((char*)buffers->buffer(), buffer, buffers->length());
            numCopied += buffers->length();
            length -= buffers->length();
            buffer += buffers->length();
            ++buffers;
        }
        else {
            bsl::memcpy((char*)buffers->buffer(), buffer, length);
            numCopied += length;
            length = 0;
        }
    }
    return numCopied;
}

}  // close unnamed namespace

                         // --------------------
                         // class btes_IovecUtil
                         // --------------------

void btes_IovecUtil::appendToBlob(bcema_Blob       *blob,
                                  const btes_Iovec *vecs,
                                  int               numVecs,
                                  int               offset)
{
    genericAppendToBlob(blob, vecs, numVecs, offset);
}

void btes_IovecUtil::appendToBlob(bcema_Blob      *blob,
                                  const btes_Ovec *vecs,
                                  int              numVecs,
                                  int              offset)
{
    genericAppendToBlob(blob, vecs, numVecs, offset);
}

bcema_Blob *btes_IovecUtil::blob(const btes_Iovec        *vecs,
                                 int                      numVecs,
                                 int                      offset,
                                 bcema_BlobBufferFactory *factory,
                                 bslma_Allocator         *allocator)
{
    bcema_Blob *blob = new(*allocator) bcema_Blob(factory, allocator);
    appendToBlob(blob, vecs, numVecs, offset);
    return blob;
}

bcema_Blob *btes_IovecUtil::blob(const btes_Iovec        *vecs,
                                 int                      numVecs,
                                 bcema_BlobBufferFactory *factory,
                                 bslma_Allocator         *allocator)
{
    bcema_Blob *blob = new(*allocator) bcema_Blob(factory, allocator);
    appendToBlob(blob, vecs, numVecs, 0);
    return blob;
}

bcema_PooledBufferChain *btes_IovecUtil::chain(const btes_Iovec *vecs,
                                               int               numVecs,
                                               int               offset,
                                 //------------^
                                 bcema_PooledBufferChainFactory *factory)
{
    return genericChain(vecs, numVecs, offset, factory);
}

bcema_PooledBufferChain *btes_IovecUtil::chain(const btes_Iovec *vecs,
                                               int               numVecs,
                                 //------------^
                                 bcema_PooledBufferChainFactory *factory)
{
    return genericChain(vecs, numVecs, 0, factory);
}

int btes_IovecUtil::gather(char             *buffer,
                           int               length,
                           const btes_Iovec *buffers,
                           int               numBuffers)
{
    return genericGather(buffer, length, buffers, numBuffers);
}

int btes_IovecUtil::gather(char            *buffer,
                           int              length,
                           const btes_Ovec *buffers,
                           int              numBuffers)
{
    return genericGather(buffer, length, buffers, numBuffers);
}

int btes_IovecUtil::length(const btes_Iovec *buffers,
                           int               numBuffers)
{
    return genericLength(buffers, numBuffers);
}

int btes_IovecUtil::length(const btes_Ovec *buffers,
                           int              numBuffers)
{
    return genericLength(buffers, numBuffers);
}

void btes_IovecUtil::pivot(int              *bufferIdx,
                           int              *offset,
                           const btes_Iovec *buffers,
                           int               numBuffers,
                           int               length)
{
    genericPivot(bufferIdx, offset, buffers, numBuffers, length);
}

void btes_IovecUtil::pivot(int             *bufferIdx,
                           int             *offset,
                           const btes_Ovec *buffers,
                           int              numBuffers,
                           int              length)
{
    genericPivot(bufferIdx, offset, buffers, numBuffers, length);
}

int btes_IovecUtil::scatter(const btes_Iovec *buffers,
                            int               numBuffers,
                            const char       *buffer,
                            int               length)
{
    return genericScatter(buffers, numBuffers, buffer, length);
}

int btes_IovecUtil::scatter(const btes_Ovec *buffers,
                            int              numBuffers,
                            const char      *buffer,
                            int              length)
{
    return genericScatter(buffers, numBuffers, buffer, length);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
