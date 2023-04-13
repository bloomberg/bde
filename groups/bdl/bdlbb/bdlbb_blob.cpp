// bdlbb_blob.cpp                                                     -*-C++-*-
#include <bdlbb_blob.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlbb_blob_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>

#include <bslalg_swaputil.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>    // INT_MAX
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {
namespace {

typedef bsl::vector<bdlbb::BlobBuffer>::iterator       BlobBufferIterator;
typedef bsl::vector<bdlbb::BlobBuffer>::const_iterator BlobBufferConstIterator;

}  // close unnamed namespace

namespace bdlbb {

                              // ================
                              // class BlobBuffer
                              // ================

// MANIPULATORS
BlobBuffer& BlobBuffer::operator=(const BlobBuffer& rhs)
{
    d_buffer = rhs.d_buffer;
    d_size   = rhs.d_size;
    return *this;
}

BlobBuffer& BlobBuffer::operator=(bslmf::MovableRef<BlobBuffer> rhs)
{
    BlobBuffer& lvalue = rhs;
    d_buffer = MoveUtil::move(lvalue.d_buffer);
    d_size   = MoveUtil::move(lvalue.d_size);

    lvalue.d_size = 0;

    return *this;
}

void BlobBuffer::reset(const bsl::shared_ptr<char>& buffer, int size)
{
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(size == 0 || buffer);

    d_buffer = buffer;
    d_size   = size;
}

void BlobBuffer::reset(bslmf::MovableRef<bsl::shared_ptr<char> > buffer,
                       int                                       size)
{
    d_buffer = MoveUtil::move(buffer);
    d_size   = size;

    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(size == 0 || d_buffer);
}

void BlobBuffer::reset()
{
    d_buffer.reset();
    d_size = 0;
}

void BlobBuffer::swap(BlobBuffer& other)
{
    bslalg::SwapUtil::swap(&this->d_buffer, &other.d_buffer);
    bslalg::SwapUtil::swap(&this->d_size, &other.d_size);
}

BlobBuffer BlobBuffer::trim(int toSize)
{
    BSLS_ASSERT(0 <= toSize && toSize <= size());

    BlobBuffer leftover(bsl::shared_ptr<char>(d_buffer, data() + toSize),
                        d_size - toSize);
    d_size = toSize;
    return leftover;
}

// ACCESSORS
bsl::ostream& BlobBuffer::print(bsl::ostream& stream, int, int) const
{
    bdlb::Print::hexDump(stream, d_buffer.get(), d_size);
    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdlbb::operator<<(bsl::ostream& stream, const BlobBuffer& buffer)
{
    return buffer.print(stream, 0, -1);
}

// FREE FUNCTIONS
void
bdlbb::swap(BlobBuffer& a, BlobBuffer& b)
{
    a.swap(b);
}


namespace bdlbb {

                          // =======================
                          // class BlobBufferFactory
                          // =======================

// CREATORS
BlobBufferFactory::~BlobBufferFactory()
{
}

                                 // ==========
                                 // class Blob
                                 // ==========

// PRIVATE ACCESSORS
int Blob::assertInvariants() const
{
    BSLS_ASSERT(0 <= d_totalSize);
    BSLS_ASSERT(0 <= d_dataLength);
    BSLS_ASSERT(d_dataLength <= d_totalSize);
    BSLS_ASSERT(-1 <= d_dataIndex);
    BSLS_ASSERT(0 <= d_preDataIndexLength);
    BSLS_ASSERT(d_preDataIndexLength <= d_dataLength);

    int       preDataLength = 0;
    int       index         = 0;
    const int NUM_BUFFERS   = numBuffers();

    for (; index < d_dataIndex; ++index) {
        preDataLength += buffer(index).size();
    }

    BSLS_ASSERT(preDataLength == d_preDataIndexLength);

    int totalSize = preDataLength;
    for (int i = index; i < NUM_BUFFERS; ++i) {
        totalSize += buffer(i).size();
    }

    BSLS_ASSERT(totalSize == d_totalSize);  (void)totalSize;

    if (-1 == d_dataIndex) {
       BSLS_ASSERT(0 == d_dataLength);
       BSLS_ASSERT(0 == d_preDataIndexLength);
    }

    if (0 < d_dataLength) {
        BSLS_ASSERT(NUM_BUFFERS != index                              );
        BSLS_ASSERT(d_dataIndex <  static_cast<int>(d_buffers.size()) );
        BSLS_ASSERT(0           <= d_dataLength - d_preDataIndexLength);
    }
    else {
        BSLS_ASSERT(0 == d_preDataIndexLength);
    }

    return 0;
}

// PRIVATE MANIPULATORS
void Blob::slowSetLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (0 == length) {
        d_dataIndex          = -1;
        d_dataLength         =  0;
        d_preDataIndexLength =  0;
        return;                                                       // RETURN
    }

    // Grow if needed.

    BSLS_ASSERT(length <= d_totalSize || d_bufferFactory_p);
    while (length > d_totalSize) {
        BlobBuffer buf;
        d_bufferFactory_p->allocate(&buf);
        appendBuffer(buf);
    }

    if (length > d_dataLength) {
        // If there is a last data buffer, add its remaining capacity.

        int currentBufferSize = 0;
        if (-1 != d_dataIndex) {
            currentBufferSize = d_buffers[d_dataIndex].size();
            d_dataLength = d_preDataIndexLength + currentBufferSize;
            BSLS_ASSERT(d_dataLength < length);
        }
        int left = length - d_dataLength;

        // Add space from additional capacity buffers until the blob is the
        // requested length.

        do {
            d_preDataIndexLength += currentBufferSize;
            ++d_dataIndex;
            currentBufferSize = d_buffers[d_dataIndex].size();
            d_dataLength += bsl::min(left, currentBufferSize);
            left -= currentBufferSize;
        } while (left > 0);
        return;                                                       // RETURN
    }

    // We are decreasing the length.

    BSLS_ASSERT(d_preDataIndexLength >= length);

    // Empty the last data buffer.

    int left     = d_preDataIndexLength - length;
    d_dataLength = d_preDataIndexLength;
    --d_dataIndex;

    // Empty the necessary amount of data buffers to get the required length.

    while (d_dataIndex >= 0 &&
           left > 0 &&
           left >= d_buffers[d_dataIndex].size()) {
        d_preDataIndexLength -= d_buffers[d_dataIndex].size();
        const int currentBufferSize = d_buffers[d_dataIndex].size();
        d_dataLength -= currentBufferSize;
        left -= currentBufferSize;
        --d_dataIndex;
    }

    // Remove the reminder.

    if (left > 0) {
        d_dataLength -= left;
    }

    if (d_dataIndex >= 0) {
        d_preDataIndexLength -= d_buffers[d_dataIndex].size();
    }
}

// CREATORS
Blob::Blob(bslma::Allocator *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(-1)
, d_preDataIndexLength(0)
, d_bufferFactory_p(0)
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(BlobBufferFactory *factory, bslma::Allocator *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(-1)
, d_preDataIndexLength(0)
, d_bufferFactory_p(factory)
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(const BlobBuffer  *buffers,
           int                numBuffers,
           BlobBufferFactory *factory,
           bslma::Allocator  *basicAllocator)
: d_buffers(buffers, buffers + numBuffers, basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(-1)
, d_preDataIndexLength(0)
, d_bufferFactory_p(factory)
{
    for (BlobBufferConstIterator it = d_buffers.begin(); it != d_buffers.end();
         ++it) {
        BSLS_ASSERT(0 <= it->size());
        d_totalSize += it->size();
    }
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(const Blob&        original,
           BlobBufferFactory *factory,
           bslma::Allocator  *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(factory)
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(const Blob& original, bslma::Allocator *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(0)
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(bslmf::MovableRef<Blob> original) BSLS_KEYWORD_NOEXCEPT
: d_buffers(MoveUtil::move(MoveUtil::access(original).d_buffers))
, d_totalSize(MoveUtil::move(MoveUtil::access(original).d_totalSize))
, d_dataLength(MoveUtil::move(MoveUtil::access(original).d_dataLength))
, d_dataIndex(MoveUtil::move(MoveUtil::access(original).d_dataIndex))
, d_preDataIndexLength(
            MoveUtil::move(MoveUtil::access(original).d_preDataIndexLength))
, d_bufferFactory_p(
            MoveUtil::move(MoveUtil::access(original).d_bufferFactory_p))
{
    Blob& lvalue = original;
    lvalue.removeAll();
}

Blob::Blob(bslmf::MovableRef<Blob> original, bslma::Allocator *basicAllocator)
: d_buffers(MoveUtil::move(MoveUtil::access(original).d_buffers),
            basicAllocator)
, d_totalSize(MoveUtil::move(MoveUtil::access(original).d_totalSize))
, d_dataLength(MoveUtil::move(MoveUtil::access(original).d_dataLength))
, d_dataIndex(MoveUtil::move(MoveUtil::access(original).d_dataIndex))
, d_preDataIndexLength(
            MoveUtil::move(MoveUtil::access(original).d_preDataIndexLength))
, d_bufferFactory_p(
            MoveUtil::move(MoveUtil::access(original).d_bufferFactory_p))
{
    Blob& lvalue = original;
    lvalue.removeAll();
}

Blob::~Blob()
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

// MANIPULATORS
Blob& Blob::operator=(const Blob& rhs)
{
    d_buffers.reserve(rhs.numBuffers());

    d_buffers            = rhs.d_buffers;
    d_totalSize          = rhs.d_totalSize;
    d_dataLength         = rhs.d_dataLength;
    d_dataIndex          = rhs.d_dataIndex;
    d_preDataIndexLength = rhs.d_preDataIndexLength;

    return *this;
}

Blob& Blob::operator=(bslmf::MovableRef<Blob> rhs)
{
    Blob& lvalue = rhs;

    d_buffers            = MoveUtil::move(lvalue.d_buffers);
    d_totalSize          = MoveUtil::move(lvalue.d_totalSize);
    d_dataLength         = MoveUtil::move(lvalue.d_dataLength);
    d_dataIndex          = MoveUtil::move(lvalue.d_dataIndex);
    d_preDataIndexLength = MoveUtil::move(lvalue.d_preDataIndexLength);
    d_bufferFactory_p    = MoveUtil::move(lvalue.d_bufferFactory_p);

    lvalue.removeAll();

    return *this;
}

void Blob::appendBuffer(bslmf::MovableRef<BlobBuffer> buffer)
{
    BlobBuffer& lvalue     = buffer;
    const int   bufferSize = lvalue.size();

    BSLS_ASSERT(d_totalSize <= INT_MAX - lvalue.size());
    BSLS_ASSERT(numBuffers() < INT_MAX);

    d_buffers.push_back(MoveUtil::move(lvalue));
    d_totalSize += bufferSize;
}

void Blob::appendDataBuffer(bslmf::MovableRef<BlobBuffer> buffer)
{
    BSLS_ASSERT(numBuffers() < INT_MAX);

    BlobBuffer& lvalue        = buffer;
    const int   bufferSize    = lvalue.size();
    const int   oldDataLength = d_dataLength;

    if (d_totalSize == d_dataLength || 0 == d_dataLength) {
        // Fast path.  At the start, we had some data buffers in the blob and
        // all non-zero sized buffers were full or there was no data, but
        // empty buffers could be present.

        BSLS_ASSERT(d_totalSize <= INT_MAX - bufferSize);

        d_buffers.insert(d_buffers.begin() + numDataBuffers(),
                         MoveUtil::move(lvalue));

        d_preDataIndexLength = oldDataLength;
        ++d_dataIndex;
        d_totalSize += bufferSize;
        d_dataLength += bufferSize;
    }
    else {
        // Complicated case -- at the start, buffer(s) with data were present,
        // trimming 'prevBuf' might or might not be necessary, empty space was
        // present on the end, whole empty buffer(s) might or might not have
        // been present on the end.
        //
        // The last data buffer can be trimmed during appending new buffer.
        // Therefore, the potentially allowed size of the added buffer should
        // be adjusted accordingly.

        const int TRIMMED_SIZE =
            d_buffers[d_dataIndex].size() - lastDataBufferLength();

        BSLS_ASSERT(d_totalSize - TRIMMED_SIZE <= INT_MAX - bufferSize);
        BSLS_ASSERT(d_dataLength > 0);
        BSLS_ASSERT(d_dataLength < d_totalSize);
        BSLS_ASSERT((unsigned)d_dataIndex < d_buffers.size());
        BSLS_ASSERT(oldDataLength >= d_preDataIndexLength);

        BlobBuffer&    prevBuf        = d_buffers[d_dataIndex];
        const unsigned newPrevBufSize = oldDataLength - d_preDataIndexLength;

        BSLS_ASSERT(TRIMMED_SIZE <= static_cast<int>(prevBuf.size()));

        prevBuf.setSize(newPrevBufSize);
        d_totalSize -= TRIMMED_SIZE;

        d_buffers.insert(d_buffers.begin() + d_dataIndex + 1,
                         MoveUtil::move(lvalue));
        ++d_dataIndex;
        d_preDataIndexLength = oldDataLength;
        d_totalSize += bufferSize;
        d_dataLength += bufferSize;
    }
}

void Blob::insertBuffer(int index, bslmf::MovableRef<BlobBuffer> buffer)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index <= static_cast<int>(d_buffers.size()));

    BlobBuffer& lvalue     = buffer;
    const int   bufferSize = lvalue.size();
    BSLS_ASSERT(d_totalSize <= INT_MAX - bufferSize);
    BSLS_ASSERT(numBuffers() < INT_MAX);

    d_buffers.insert(d_buffers.begin() + index, MoveUtil::move(lvalue));

    d_totalSize += bufferSize;
    if (0 != d_dataLength && index <= d_dataIndex) {
        // Newly-inserted buffer is a data buffer.

        d_dataLength += bufferSize;
        d_preDataIndexLength += bufferSize;
        ++d_dataIndex;
    }
}

void Blob::prependDataBuffer(bslmf::MovableRef<BlobBuffer> buffer)
{
    BlobBuffer& lvalue     = buffer;
    const int   bufferSize = lvalue.size();

    BSLS_ASSERT(d_totalSize <= INT_MAX - bufferSize);
    BSLS_ASSERT(numBuffers() < INT_MAX);

    d_buffers.insert(d_buffers.begin(), MoveUtil::move(lvalue));

    ++d_dataIndex;
    if (0 != d_dataIndex) {
        d_preDataIndexLength += bufferSize;
    }
    d_totalSize += bufferSize;
    d_dataLength += bufferSize;
}

void Blob::removeAll()
{
    d_buffers.clear();
    d_totalSize          =  0;
    d_dataLength         =  0;
    d_dataIndex          = -1;
    d_preDataIndexLength =  0;
}

void Blob::removeBuffer(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < static_cast<int>(d_buffers.size()));

    d_totalSize -= d_buffers[index].size();
    if (d_dataIndex == index) {
        d_dataLength = d_preDataIndexLength;

        if (d_preDataIndexLength != 0) {
            d_preDataIndexLength -= d_buffers[index - 1].size();
        }

        --d_dataIndex;
    }
    else if (d_dataIndex > index) {
        d_preDataIndexLength -= d_buffers[index].size();
        d_dataLength -= d_buffers[index].size();
        --d_dataIndex;
    }
    d_buffers.erase(d_buffers.begin() + index);
}

void Blob::removeBuffers(int index, int numBuffers)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numBuffers);
    BSLS_ASSERT(index + numBuffers <= this->numBuffers());

    // As we iterate through the buffers the values of the class data members
    // will change constantly making it difficult to iterate correctly.  Make a
    // local copy of the variables so we can iterate through the blob and
    // update the data members at the end.

    int dataIndex          = d_dataIndex;
    int dataLength         = d_dataLength;
    int totalSize          = d_totalSize;
    int preDataIndexLength = d_preDataIndexLength;

    for (int i = 0; i < numBuffers; ++i) {
        const int currIdx = index + i;
        const int bufSize = d_buffers[currIdx].size();

        totalSize -= bufSize;
        if (currIdx < d_dataIndex) {
            preDataIndexLength -= bufSize;
            dataLength -= bufSize;
            --dataIndex;
        }
        else if (currIdx == d_dataIndex) {
            dataLength = preDataIndexLength;

            if (preDataIndexLength != 0) {
                BSLS_ASSERT(0 != dataIndex);

                preDataIndexLength -= d_buffers[dataIndex - 1].size();
            }

            --dataIndex;
        }
    }

    d_buffers.erase(d_buffers.begin() + index,
                    d_buffers.begin() + index + numBuffers);

    d_dataIndex          = dataIndex;
    d_dataLength         = dataLength;
    d_totalSize          = totalSize;
    d_preDataIndexLength = preDataIndexLength;
}

void Blob::removeUnusedBuffers()
{
    if (numDataBuffers() < numBuffers()) {
        d_totalSize = d_dataLength > 0 ? d_preDataIndexLength +
                                             d_buffers[d_dataIndex].size()
                                       : 0;

        d_buffers.erase(d_buffers.begin() + numDataBuffers(), d_buffers.end());
    }
}

void Blob::replaceDataBuffer(int index, const BlobBuffer& buffer)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < numDataBuffers());
    BSLS_ASSERT(d_totalSize <= INT_MAX - buffer.size());

    int sizeDiff = buffer.size() - d_buffers[index].size();

    d_totalSize += sizeDiff;
    if (d_dataIndex == index) {
        d_dataLength = d_preDataIndexLength + buffer.size();
    }
    else if (d_dataIndex > index) {
        d_preDataIndexLength += sizeDiff;
        d_dataLength += sizeDiff;
    }

    d_buffers[index] = buffer;
}

void Blob::setLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (d_dataLength == length) {
        return;                                                       // RETURN
    }

    if (d_totalSize &&
        -1 != d_dataIndex &&
        d_preDataIndexLength + d_buffers[d_dataIndex].size() >= length &&
        d_preDataIndexLength < length) {
        // We are not crossing any buffer boundaries.

        d_dataLength = length;
        return;                                                       // RETURN
    }

    return slowSetLength(length);
}

void Blob::swap(Blob& other)
{
    BSLS_ASSERT(this->allocator() == other.allocator());

    bslalg::SwapUtil::swap(&this->d_buffers, &other.d_buffers);
    bslalg::SwapUtil::swap(&this->d_totalSize, &other.d_totalSize);
    bslalg::SwapUtil::swap(&this->d_dataLength, &other.d_dataLength);
    bslalg::SwapUtil::swap(&this->d_dataIndex, &other.d_dataIndex);
    bslalg::SwapUtil::swap(&this->d_preDataIndexLength,
                           &other.d_preDataIndexLength);
    bslalg::SwapUtil::swap(&this->d_bufferFactory_p, &other.d_bufferFactory_p);
}

void Blob::swapBufferRaw(int index, BlobBuffer *srcBuffer)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < numBuffers());
    BSLS_ASSERT(srcBuffer->size() == buffer(index).size());

    d_buffers[index].buffer().swap(srcBuffer->buffer());
}

BlobBuffer Blob::trimLastDataBuffer()
{
    if (0 == d_dataLength) {
        return BlobBuffer();                                          // RETURN
    }

    BlobBuffer& lastBuffer       = d_buffers[d_dataIndex];
    int         lastBufferLength = lastDataBufferLength();
    d_totalSize -= lastBuffer.size() - lastBufferLength;
    return lastBuffer.trim(lastBufferLength);
}

void Blob::moveBuffers(Blob *srcBlob)
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

void Blob::moveDataBuffers(Blob *srcBlob)
{
    if (0 == srcBlob->length()) {
        d_dataIndex          = -1;
        d_dataLength         =  0;
        d_preDataIndexLength =  0;
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
    d_totalSize = d_preDataIndexLength + d_buffers[d_dataIndex].size();

    srcBlob->d_buffers.erase(srcBlob->d_buffers.begin(),
                             srcBlob->d_buffers.begin() + numSrcDataBuffers);

    srcBlob->d_dataIndex          = -1;
    srcBlob->d_dataLength         =  0;
    srcBlob->d_preDataIndexLength =  0;
    srcBlob->d_totalSize -= d_totalSize;
}

void Blob::moveAndAppendDataBuffers(Blob *srcBlob)
{
    if (0 == srcBlob->length()) {
        return;                                                       // RETURN
    }

    trimLastDataBuffer();  // Note that this call may update 'd_totalSize'.

    BSLS_ASSERT(d_totalSize <= INT_MAX - srcBlob->totalSize());
    BSLS_ASSERT(numBuffers() <= INT_MAX - srcBlob->numBuffers());

    const int numSrcDataBuffers = srcBlob->numDataBuffers();
    const int numDstDataBuffers = numDataBuffers();

    reserveBufferCapacity(static_cast<int>(d_buffers.size()) +
                          numSrcDataBuffers);

    BlobBufferIterator dstIter = d_buffers.begin() + numDstDataBuffers;
    BlobBufferIterator srcIter = srcBlob->d_buffers.begin();

    d_buffers.insert(dstIter, numSrcDataBuffers, BlobBuffer());

    for (int i = 0; i < numSrcDataBuffers; ++i, ++srcIter, ++dstIter) {
        dstIter->buffer().swap(srcIter->buffer());
        dstIter->setSize(srcIter->size());
    }

    const int totalSizeAdded =
        srcBlob->d_preDataIndexLength +
        srcBlob->d_buffers[numSrcDataBuffers - 1].size();

    d_dataIndex = 0 == length() ? numSrcDataBuffers - 1
                                : d_dataIndex + numSrcDataBuffers;
    d_preDataIndexLength = d_dataLength + srcBlob->d_preDataIndexLength;
    d_dataLength += srcBlob->d_dataLength;
    d_totalSize += totalSizeAdded;

    srcBlob->d_buffers.erase(srcBlob->d_buffers.begin(),
                             srcBlob->d_buffers.begin() + numSrcDataBuffers);

    srcBlob->d_dataIndex          = -1;
    srcBlob->d_dataLength         =  0;
    srcBlob->d_preDataIndexLength =  0;
    srcBlob->d_totalSize -= totalSizeAdded;
}
}  // close package namespace

// FREE OPERATORS
bool bdlbb::operator==(const Blob& lhs, const Blob& rhs)
{
    return lhs.d_buffers == rhs.d_buffers &&
           lhs.d_totalSize == rhs.d_totalSize &&
           lhs.d_dataLength == rhs.d_dataLength &&
           lhs.d_dataIndex == rhs.d_dataIndex &&
           lhs.d_preDataIndexLength == rhs.d_preDataIndexLength;
}

bool bdlbb::operator!=(const Blob& lhs, const Blob& rhs)
{
    return lhs.d_buffers != rhs.d_buffers ||
           lhs.d_totalSize != rhs.d_totalSize ||
           lhs.d_dataLength != rhs.d_dataLength ||
           lhs.d_dataIndex != rhs.d_dataIndex ||
           lhs.d_preDataIndexLength != rhs.d_preDataIndexLength;
}

// FREE FUNCTIONS
void
bdlbb::swap(Blob& a, Blob& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);
    }
    else {
        typedef bslmf::MovableRefUtil MoveUtil;
        Blob x(MoveUtil::move(a), a.allocator());
        a = b;
        b = x;
    }
}

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
