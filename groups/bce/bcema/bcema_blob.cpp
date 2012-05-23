// bcema_blob.cpp                                                     -*-C++-*-
#include <bcema_blob.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_blob_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_algorithm.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM__CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {

namespace {

typedef bsl::vector<bcema_BlobBuffer>::iterator       BlobBufferIterator;
typedef bsl::vector<bcema_BlobBuffer>::const_iterator BlobBufferConstIterator;

                       // ==============================
                       // class InvalidBlobBufferFactory
                       // ==============================

class InvalidBlobBufferFactory : private bcema_BlobBufferFactory {

    // NOT IMPLEMENTED
    InvalidBlobBufferFactory(const InvalidBlobBufferFactory&);
    InvalidBlobBufferFactory& operator=(const InvalidBlobBufferFactory&);

  private:
    // PRIVATE CREATORS
    InvalidBlobBufferFactory()  { }
    ~InvalidBlobBufferFactory() { }

  public:
    // CLASS METHODS
    static
    InvalidBlobBufferFactory& singleton()
    {
        static InvalidBlobBufferFactory instance;
        return instance;
    }

    static
    bcema_BlobBufferFactory *factory(bcema_BlobBufferFactory *basicFactory = 0)
    {
        return basicFactory ? basicFactory : &singleton();
    }

    virtual void allocate(bcema_BlobBuffer *)
    {
        BSLS_ASSERT_OPT(!"Invalid Blob Buffer Factory Used!");
    }
};

}  // close unnamed namespace

                           // ======================
                           // class bcema_BlobBuffer
                           // ======================

// MANIPULATORS
bcema_BlobBuffer& bcema_BlobBuffer::operator=(const bcema_BlobBuffer& rhs)
{
    d_buffer = rhs.d_buffer;
    d_size   = rhs.d_size;
    return *this;
}

void bcema_BlobBuffer::reset(const bcema_SharedPtr<char>& buffer, int size)
{
    d_buffer = buffer;
    d_size   = size;
}

void bcema_BlobBuffer::reset()
{
    d_buffer.clear();
    d_size = 0;
}

// ACCESSORS
bsl::ostream& bcema_BlobBuffer::print(bsl::ostream& stream, int, int) const
{
    bdeu_Print::hexDump(stream, d_buffer.ptr(), d_size);
    return stream << bsl::flush;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bcema_BlobBuffer& buffer)
{
    return buffer.print(stream, 0, -1);
}

                        // =============================
                        // class bcema_BlobBufferFactory
                        // =============================

// CREATORS
bcema_BlobBufferFactory::~bcema_BlobBufferFactory()
{
}

                              // ================
                              // class bcema_Blob
                              // ================

// PRIVATE ACCESSORS
int bcema_Blob::assertInvariants() const
{
    BSLS_ASSERT(0 != d_bufferFactory_p);
    BSLS_ASSERT(0 <= d_totalSize);
    BSLS_ASSERT(0 <= d_dataLength);
    BSLS_ASSERT(d_dataLength <= d_totalSize);
    BSLS_ASSERT(0 <= d_dataIndex);
    BSLS_ASSERT(0 <= d_preDataIndexLength);
    BSLS_ASSERT(d_preDataIndexLength <= d_dataLength);

    int preDataLength = 0;
    BlobBufferConstIterator dataIter;
    for (dataIter = d_buffers.begin();
         dataIter != d_buffers.end();
         ++dataIter) {
        if (preDataLength + dataIter->size() >= d_dataLength) {
            break;
        }
        preDataLength += dataIter->size();
    }
    BSLS_ASSERT(preDataLength == d_preDataIndexLength);
    BSLS_ASSERT(dataIter - d_buffers.begin() == d_dataIndex);

    int totalSize = preDataLength;
    for (BlobBufferConstIterator it = dataIter; it != d_buffers.end(); ++it) {
        totalSize += it->size();
    }
    BSLS_ASSERT(totalSize == d_totalSize);

    if (0 < d_dataLength) {
        BSLS_ASSERT(d_buffers.end() != dataIter);
        BSLS_ASSERT(d_dataIndex == dataIter - d_buffers.begin());
        BSLS_ASSERT(d_dataIndex < static_cast<int>(d_buffers.size()));
        BSLS_ASSERT(0 < d_dataLength - d_preDataIndexLength);
        BSLS_ASSERT(d_dataLength - d_preDataIndexLength <=
                                                d_buffers[d_dataIndex].size());

        // If 'it != d_buffers.end()', then the following assertion is implied
        // by the loop above since we had to break; on the other hand, if
        // 'it == d_buffers.end()', then 'it->size()' has no meaning.
        //..
        //  BSLS_ASSERT(d_dataLength - d_preDataIndexLength < it->size());
        //..
    } else {
        BSLS_ASSERT(0 == d_preDataIndexLength);
    }

    return 0;
}

// PRIVATE MANIPULATORS
void bcema_Blob::slowSetLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (0 == length) {
        d_dataIndex          = 0;
        d_dataLength         = 0;
        d_preDataIndexLength = 0;
        return;                                                       // RETURN
    }

    // Grow if needed.

    bcema_BlobBuffer buf;
    while (length > d_totalSize) {
        d_bufferFactory_p->allocate(&buf);
        appendBuffer(buf);
    }

    if (1 == d_buffers.size()) {
        // This is a corner case.  We are not crossing any buffer boundaries
        // because we have only one buffer.  The rest of the code assumes that
        // 'd_dataIndex' will need to change.

        d_dataLength = length;
        return;                                                       // RETURN
    }

    if (length > d_dataLength) {
        // Move 'd_dataLength' to the beginning of the next buffer.

        d_dataLength = d_preDataIndexLength + d_buffers[d_dataIndex].size();
        BSLS_ASSERT(d_dataLength < length);

        int left = length - d_dataLength;
        do {
            d_preDataIndexLength += d_buffers[d_dataIndex].size();
            ++d_dataIndex;

            d_dataLength += bsl::min(left, d_buffers[d_dataIndex].size());
            left -= d_buffers[d_dataIndex].size();
        } while (left > 0);
        return;                                                       // RETURN
    }

    // We are decreasing the length.

    BSLS_ASSERT(d_preDataIndexLength >= length);
    int left = d_preDataIndexLength - length;
    d_dataLength = d_preDataIndexLength;

    do {
        --d_dataIndex;
        d_preDataIndexLength -= d_buffers[d_dataIndex].size();

        d_dataLength -= bsl::min(left, d_buffers[d_dataIndex].size());
        left -= d_buffers[d_dataIndex].size();
    } while (left >= 0);
}

// CREATORS
bcema_Blob::bcema_Blob(bslma_Allocator *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(0)
, d_preDataIndexLength(0)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(0))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

bcema_Blob::bcema_Blob(bcema_BlobBufferFactory *factory,
                       bslma_Allocator         *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(0)
, d_preDataIndexLength(0)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(factory))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

bcema_Blob::bcema_Blob(const bcema_BlobBuffer  *buffers,
                       int                      numBuffers,
                       bcema_BlobBufferFactory *factory,
                       bslma_Allocator         *basicAllocator)
: d_buffers(buffers, buffers + numBuffers, basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(0)
, d_preDataIndexLength(0)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(factory))
{
    for (BlobBufferConstIterator it = d_buffers.begin();
         it != d_buffers.end(); ++it) {
        BSLS_ASSERT(0 <= it->size());
        d_totalSize += it->size();
    }
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

bcema_Blob::bcema_Blob(const bcema_Blob&        original,
                       bcema_BlobBufferFactory *factory,
                       bslma_Allocator         *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(factory))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

bcema_Blob::bcema_Blob(const bcema_Blob&  original,
                       bslma_Allocator   *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(0))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

bcema_Blob::~bcema_Blob()
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

// MANIPULATORS
bcema_Blob& bcema_Blob::operator=(const bcema_Blob& rhs)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    d_buffers.reserve(rhs.numBuffers());

    d_buffers            = rhs.d_buffers;
    d_totalSize          = rhs.d_totalSize;
    d_dataLength         = rhs.d_dataLength;
    d_dataIndex          = rhs.d_dataIndex;
    d_preDataIndexLength = rhs.d_preDataIndexLength;
    return *this;
}

void bcema_Blob::appendBuffer(const bcema_BlobBuffer& buffer)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    d_buffers.push_back(buffer);
    d_totalSize += buffer.size();
}

void bcema_Blob::appendDataBuffer(const bcema_BlobBuffer& buffer)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    const int bufferSize = buffer.size();

    if (d_totalSize == d_dataLength) {
        // Fast path.  We have 0 or more data buffers in the blob and they are
        // all full.
    
        d_buffers.push_back(buffer);
        d_preDataIndexLength = d_dataLength;
        d_totalSize  += bufferSize;
        d_dataLength += bufferSize;
        d_dataIndex = d_buffers.size() - 1;
    }
    else if (0 == d_dataLength) {
        BSLS_ASSERT_SAFE(0 != d_totalSize);

        // Another fast path.  No data, but empty buffers are present.  Put the
        // new buffer at the front.

        BSLS_ASSERT(0 == d_dataIndex);
        BSLS_ASSERT(0 == d_preDataIndexLength);
        d_buffers.insert(d_buffers.begin(), buffer);
        d_totalSize += bufferSize;
        d_dataLength = bufferSize;
    }
    else {
        BSLS_ASSERT_SAFE(d_dataLength > 0);
        BSLS_ASSERT_SAFE(d_totalSize > d_dataLength);

        // Complicated case -- buffer(s) with data exist, trimming
        // 'lastDataBuf' migth or might not be necessary, empty buffer(s) might
        // or might not be present on the end.

        BSLS_ASSERT_SAFE((unsigned) d_dataIndex < d_buffers.size());
        bcema_BlobBuffer& lastDataBuf = d_buffers[d_dataIndex];
        int lastDataBufSize = lastDataBuf.size();
        int trim = lastDataBufSize - (d_dataLength - d_preDataIndexLength);
        BSLS_ASSERT(trim < lastDataBufSize);  BSLS_ASSERT(trim >= 0);

        lastDataBuf.setSize(lastDataBufSize - trim);

        ++d_dataIndex;
        d_buffers.insert(d_buffers.begin() + d_dataIndex, buffer);
        d_preDataIndexLength = d_dataLength;
        d_totalSize  += bufferSize - trim;
        d_dataLength += bufferSize;
    }
}

void bcema_Blob::insertBuffer(int index, const bcema_BlobBuffer& buffer)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index <= static_cast<int>(d_buffers.size()));

    const int bufferSize = buffer.size();
    d_buffers.insert(d_buffers.begin() + index, buffer);
    d_totalSize += bufferSize;
    if (0 != d_dataLength && index <= d_dataIndex) {
        // Newly-inserted buffer is a data buffer.

        d_dataLength         += bufferSize;
        d_preDataIndexLength += bufferSize;
        ++d_dataIndex;
    }
}

void bcema_Blob::prependDataBuffer(const bcema_BlobBuffer& buffer)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    const int bufferSize = buffer.size();
    BSLS_ASSERT(0 < bufferSize);
    d_buffers.insert(d_buffers.begin(), buffer);
    if (0 != d_dataLength) {
        ++d_dataIndex;
        d_preDataIndexLength += bufferSize;
    }
    d_totalSize  += bufferSize;
    d_dataLength += bufferSize;
}

void bcema_Blob::removeAll()
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    d_buffers.clear();
    d_totalSize  = 0;
    d_dataLength = 0;
    d_dataIndex  = 0;
    d_preDataIndexLength = 0;
}

void bcema_Blob::removeBuffer(int index)
{
    // BSLS_ASSERT_SAFE(0 == assertInvariants());

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < static_cast<int>(d_buffers.size()));

    d_totalSize -= d_buffers[index].size();
    if (d_dataIndex == index) {
        d_dataLength = d_preDataIndexLength;

        if (d_preDataIndexLength != 0) {
            d_preDataIndexLength -= d_buffers[index - 1].size();
        }

        // In the case of an empty blob, d_dataIndex is 0.

        if (d_dataLength > 0) {
            --d_dataIndex;
        }
    }
    else if (d_dataIndex > index) {
        d_preDataIndexLength -= d_buffers[index].size();
        d_dataLength -= d_buffers[index].size();
        --d_dataIndex;
    }
    d_buffers.erase(d_buffers.begin() + index);
}

void bcema_Blob::setLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (d_totalSize
     && d_preDataIndexLength + d_buffers[d_dataIndex].size() >= length
     && d_preDataIndexLength < length) {
        // We are not crossing any buffer boundaries.

        d_dataLength = length;
        return;
    }

    return slowSetLength(length);
}

void bcema_Blob::swapBufferRaw(int index, bcema_BlobBuffer *srcBuffer)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < numBuffers());
    BSLS_ASSERT(srcBuffer->size() == buffer(index).size());

    d_buffers[index].buffer().swap(srcBuffer->buffer());
}

void bcema_Blob::trimLastDataBuffer()
{
    if (0 != d_dataLength
     && d_dataLength - d_preDataIndexLength < d_buffers[d_dataIndex].size()) {
        d_totalSize -= d_buffers[d_dataIndex].size();
        d_buffers[d_dataIndex].setSize(d_dataLength - d_preDataIndexLength);
        d_totalSize += d_dataLength - d_preDataIndexLength;
    }
}

void bcema_Blob::moveBuffers(bcema_Blob *srcBlob)
{
    d_buffers.resize(srcBlob->d_buffers.size());

    BlobBufferIterator dstIter = d_buffers.begin();
    BlobBufferIterator srcIter = srcBlob->d_buffers.begin();
    for (; srcIter != srcBlob->d_buffers.end(); ++srcIter, ++dstIter) {
        dstIter->buffer().swap(srcIter->buffer());
        dstIter->setSize(srcIter->size());
    }
    d_totalSize          = srcBlob->d_totalSize;
    d_dataLength         = srcBlob->d_dataLength;
    d_dataIndex          = srcBlob->d_dataIndex;
    d_preDataIndexLength = srcBlob->d_preDataIndexLength;
    srcBlob->removeAll();
}

void bcema_Blob::moveDataBuffers(bcema_Blob *srcBlob)
{
    if (0 == srcBlob->length()) {
        d_dataIndex          = 0;
        d_dataLength         = 0;
        d_preDataIndexLength = 0;
        return;                                                       // RETURN
    }

    const int numSrcDataBuffers = srcBlob->numDataBuffers();

    d_buffers.resize(numSrcDataBuffers);

    BlobBufferIterator dstIter = d_buffers.begin();
    BlobBufferIterator srcIter = srcBlob->d_buffers.begin();

    for (int i = 0; i < numSrcDataBuffers; ++i, ++srcIter, ++dstIter) {
        dstIter->buffer().swap(srcIter->buffer());
        dstIter->setSize(srcIter->size());
    }

    d_dataIndex          = srcBlob->d_dataIndex;
    d_dataLength         = srcBlob->d_dataLength;
    d_preDataIndexLength = srcBlob->d_preDataIndexLength;
    d_totalSize          = d_preDataIndexLength
                                               + d_buffers[d_dataIndex].size();

    srcBlob->d_buffers.erase(srcBlob->d_buffers.begin(),
                             srcBlob->d_buffers.begin() + numSrcDataBuffers);

    srcBlob->d_dataIndex           = 0;
    srcBlob->d_dataLength          = 0;
    srcBlob->d_preDataIndexLength  = 0;
    srcBlob->d_totalSize          -= d_totalSize;
}

void bcema_Blob::moveAndAppendDataBuffers(bcema_Blob *srcBlob)
{
    if (0 == srcBlob->length()) {
        return;                                                       // RETURN
    }

    trimLastDataBuffer();  // Note that this call may update 'd_totalSize'.

    const int numSrcDataBuffers = srcBlob->numDataBuffers();
    const int numDstDataBuffers = numDataBuffers();

    reserveBufferCapacity(d_buffers.size() + numSrcDataBuffers);

    BlobBufferIterator dstIter = d_buffers.begin() + numDstDataBuffers;
    BlobBufferIterator srcIter = srcBlob->d_buffers.begin();

    d_buffers.insert(dstIter, numSrcDataBuffers, bcema_BlobBuffer());

    for (int i = 0; i < numSrcDataBuffers; ++i, ++srcIter, ++dstIter) {
        dstIter->buffer().swap(srcIter->buffer());
        dstIter->setSize(srcIter->size());
    }

    const int totalSizeAdded = srcBlob->d_preDataIndexLength +
                              srcBlob->d_buffers[numSrcDataBuffers - 1].size();

    d_dataIndex          = 0 == length()
                           ? numSrcDataBuffers - 1
                           : d_dataIndex + numSrcDataBuffers;
    d_preDataIndexLength  = d_dataLength + srcBlob->d_preDataIndexLength;
    d_dataLength         += srcBlob->d_dataLength;
    d_totalSize          += totalSizeAdded;

    srcBlob->d_buffers.erase(srcBlob->d_buffers.begin(),
                             srcBlob->d_buffers.begin() + numSrcDataBuffers);

    srcBlob->d_dataIndex           = 0;
    srcBlob->d_dataLength          = 0;
    srcBlob->d_preDataIndexLength  = 0;
    srcBlob->d_totalSize          -= totalSizeAdded;
}

// FREE OPERATORS
bool operator==(const bcema_Blob& lhs, const bcema_Blob& rhs)
{
    return lhs.d_buffers            == rhs.d_buffers
        && lhs.d_totalSize          == rhs.d_totalSize
        && lhs.d_dataLength         == rhs.d_dataLength
        && lhs.d_dataIndex          == rhs.d_dataIndex
        && lhs.d_preDataIndexLength == rhs.d_preDataIndexLength;
}

bool operator!=(const bcema_Blob& lhs, const bcema_Blob& rhs)
{
    return lhs.d_buffers            != rhs.d_buffers
        || lhs.d_totalSize          != rhs.d_totalSize
        || lhs.d_dataLength         != rhs.d_dataLength
        || lhs.d_dataIndex          != rhs.d_dataIndex
        || lhs.d_preDataIndexLength != rhs.d_preDataIndexLength;
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
