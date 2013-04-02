// btemt_blobutil.cpp                -*-C++-*-
#include <btemt_blobutil.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <btemt_message.h>
#include <btes_iovec.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace btemt {

                        // --------------
                        // class BlobUtil
                        // --------------

// CLASS METHODS
void BlobUtil::copyOut(char              *buffer,
                       const bcema_Blob&  blob,
                       int                numBytes,
                       int                offset)
{
    BSLS_ASSERT(buffer || 0 == numBytes);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset + numBytes <= blob.length());

    const char *data = getData(buffer, numBytes, blob, offset);
    if (data != buffer) {
        memcpy(buffer, data, numBytes);
    }
}

void BlobUtil::copyToVector(bsl::vector<char> *result,
                            const bcema_Blob&  blob)
{
    result->resize(blob.length());
    copyOut(result->data(), blob, blob.length(), 0);
}

void BlobUtil::extract(char *buffer, int numBytes, bcema_Blob *blob)
{
    BSLS_ASSERT(buffer || 0 == numBytes);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(blob);
    BSLS_ASSERT(numBytes <= blob->length());

    copyOut(buffer, *blob, numBytes);
    bcema_BlobUtil::erase(blob, 0, numBytes);
}

const char *BlobUtil::getData(char              *storage,
                              int                numBytes,
                              const bcema_Blob&  blob,
                              int                blobOffset)
{
    BSLS_ASSERT(storage || 0 == numBytes);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= blobOffset);
    BSLS_ASSERT(numBytes <= blob.length() - blobOffset);
  
    char *ptr = storage;
    bool usingStorage = false;
    for (int i = 0; i < blob.numDataBuffers(); ++i)
    {
        const bcema_BlobBuffer& blobBuffer = blob.buffer(i);

        if (blobOffset >= blobBuffer.size()) {
            blobOffset -= blobBuffer.size();
            continue;
        }

        if (!usingStorage) {
            // If possible, just return a pointer to the first buffer.
            if (blobBuffer.size() - blobOffset >= numBytes) {
                return blobBuffer.data() + blobOffset;
            }
        }

        // We must use the storage, what we want isn't contiguous in a buffer
        usingStorage = true;
        int copyLen = bsl::min(numBytes, blobBuffer.size() - blobOffset);
        memcpy(ptr, blobBuffer.data() + blobOffset, copyLen);

        numBytes -= copyLen;
        ptr += copyLen;
        blobOffset = 0;
    }

    BSLS_ASSERT(numBytes == 0);
    return storage;
}

void BlobUtil::prepend(bcema_Blob        *dest,
                       const bcema_Blob&  source)
{
    bcema_BlobUtil::insert(dest, 0, source);
}

void BlobUtil::prepend(bcema_Blob        *dest,
                       const bcema_Blob&  source,
                       int                offset)
{
    bcema_BlobUtil::insert(dest, 0, source, offset);
}

void BlobUtil::prepend(bcema_Blob        *dest,
                       const bcema_Blob&  source,
                       int                offset,
                       int                length)
{
    bcema_BlobUtil::insert(dest, 0, source, offset, length);
}

void BlobUtil::prepend(bcema_Blob * /*dest*/,
                       const char * /*source*/,
                       int          /*length*/)
{
    BSLS_ASSERT(false); // not impl
    //bcema_BlobUtil::insert(dest, 0, )
}

int BlobUtil::dataBufferSize(const bcema_Blob& blob,
                             int               bufferIndex)
{
    BSLS_ASSERT(bufferIndex < blob.numDataBuffers());
    if (blob.numDataBuffers() > 1
        && bufferIndex < (blob.numDataBuffers() - 1)) {
        return blob.buffer(bufferIndex).size();
    }
    return blob.lastDataBufferLength();
}

void BlobUtil::getDataBuffer(char **buf,
    int *bufLength,
    const bcema_Blob& blob,
    int bufferIndex)
{
    BSLS_ASSERT(bufferIndex < blob.numDataBuffers());
    const bcema_BlobBuffer& blobBuffer = blob.buffer(bufferIndex);
    *buf = blobBuffer.data();
    *bufLength = dataBufferSize(blob, bufferIndex);
}

char *BlobUtil::appendEmptyBuffer(bcema_Blob       *blob,
                                  int               size,
                                  bslma::Allocator  *a)
{
    BSLS_ASSERT(blob);
    bslma::Allocator *allocator = bslma::Default::allocator(a);
    bcema_SharedPtr<char> buf((char*)allocator->allocate(size), allocator);
    bcema_BlobBuffer blobBuffer(buf, size);
    blob->trimLastDataBuffer(); // to address a bug in bcema_Blob::appendDataBuffer
    blob->appendDataBuffer(blobBuffer);
    return buf.ptr();
}

void BlobUtil::generateEmptyBuffer(char                    **buffer,
                                   int                      *size,
                                   bcema_Blob               *blob,
                                   bcema_BlobBufferFactory  *blobBufferFactory)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(size);
    BSLS_ASSERT(blob);
    BSLS_ASSERT(blobBufferFactory);

    if (blob->length() == blob->totalSize()) {
        // blob is at capacity, allocate a new empty buffer
        bcema_BlobBuffer newBuffer;
        blobBufferFactory->allocate(&newBuffer);
        blob->appendBuffer(newBuffer);
    }

    if (0 == blob->length()) {
        // no data buffers, use first buffer
        BSLS_ASSERT(blob->numBuffers() > 0);
        const bcema_BlobBuffer& firstBuffer = blob->buffer(0);
        *buffer = firstBuffer.data();
        *size = firstBuffer.size();
        return;
    }

    BSLS_ASSERT(0 < blob->numDataBuffers());
    int bufferIndex = blob->numDataBuffers() - 1;
    const bcema_BlobBuffer& lastDataBuffer = blob->buffer(bufferIndex);
    int lastDataBufferLength = blob->lastDataBufferLength();
    if (lastDataBufferLength < lastDataBuffer.size()) {
        // last data buffer is not full
        *buffer = lastDataBuffer.data() + lastDataBufferLength;
        *size = lastDataBuffer.size() - lastDataBufferLength;
    }
    else {
        // last data buffer is full, but there are empty buffers
        BSLS_ASSERT(blob->lastDataBufferLength() == lastDataBufferLength);
        ++bufferIndex;
        BSLS_ASSERT(bufferIndex < blob->numBuffers());
        const bcema_BlobBuffer& firstEmptyBuffer = blob->buffer(bufferIndex);
        BSLS_ASSERT(firstEmptyBuffer.size() > 0);
        *buffer = firstEmptyBuffer.data();
        *size = firstEmptyBuffer.size();
    }
}


BlobUtil::iterator BlobUtil::beginIterator(bcema_Blob *blob)
{
    return iterator(BlobUtil_IteratorImp<char>(blob, 0, 0));
}

BlobUtil::iterator BlobUtil::endIterator(bcema_Blob *blob)
{
    return iterator(BlobUtil_IteratorImp<char>(blob,
                                               blob->numDataBuffers(),
                                               0));
}

BlobUtil::const_iterator BlobUtil::beginIterator(const bcema_Blob *blob)
{
    return const_iterator(BlobUtil_IteratorImp<char>(blob, 0, 0));
}

BlobUtil::const_iterator BlobUtil::endIterator(const bcema_Blob *blob)
{
    return const_iterator(BlobUtil_IteratorImp<char>(blob,
                                                     blob->numDataBuffers(),
                                                     0));
}

void BlobUtil::loadOvec(bsl::vector<btes_Ovec> *vec, const bcema_Blob& blob)
{
    int numDataBuffers = blob.numDataBuffers();
    vec->resize(numDataBuffers);
    if (0 == numDataBuffers) {
        return;
    }
    int lastDataBufferLength = blob.lastDataBufferLength();
    for (int i = 0; i < numDataBuffers - 1; ++i) {
        (*vec)[i].setBuffer(blob.buffer(i).data(), blob.buffer(i).size());
    }
    (*vec)[numDataBuffers - 1].setBuffer(blob.buffer(numDataBuffers -1).data(),
                                         lastDataBufferLength);
}

void BlobUtil::toString(bsl::string *string, const bcema_Blob& blob)
{
    // copies the data twice, could fix that
    BSLS_ASSERT(string);
    bsl::vector<char> buf(blob.length());
    copyOut(buf.data(), blob, blob.length());
    string->assign(buf.data(), buf.data() + blob.length());
}

}   // close package-level namespace

}   // close enterprise-wide namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
