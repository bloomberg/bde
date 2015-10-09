// btlb_blob.cpp                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlb_blob.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlb_blob_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_algorithm.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

// Note: on Windows -> WinDef.h:#define min(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
#undef min
#endif

namespace BloombergLP {
namespace {

typedef bsl::vector<btlb::BlobBuffer>::iterator       BlobBufferIterator;
typedef bsl::vector<btlb::BlobBuffer>::const_iterator BlobBufferConstIterator;

                      // ==============================
                      // class InvalidBlobBufferFactory
                      // ==============================

class InvalidBlobBufferFactory : private btlb::BlobBufferFactory {

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
    btlb::BlobBufferFactory *factory(btlb::BlobBufferFactory *basicFactory = 0)
    {
        return basicFactory ? basicFactory : &singleton();
    }

    virtual void allocate(btlb::BlobBuffer *)
    {
        BSLS_ASSERT_OPT(!"Invalid Blob Buffer Factory Used!");
    }
};

}  // close unnamed namespace

namespace btlb {

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

void BlobBuffer::reset(const bsl::shared_ptr<char>& buffer, int size)
{
    BSLS_ASSERT(0 <= size);

    d_buffer = buffer;
    d_size   = size;
}

void BlobBuffer::reset()
{
    d_buffer.reset();
    d_size = 0;
}

// ACCESSORS
bsl::ostream& BlobBuffer::print(bsl::ostream& stream, int, int) const
{
    bdlb::Print::hexDump(stream, d_buffer.get(), d_size);
    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& btlb::operator<<(bsl::ostream& stream, const BlobBuffer& buffer)
{
    return buffer.print(stream, 0, -1);
}

namespace btlb {

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
void Blob::slowSetLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (0 == length) {
        d_dataIndex          = 0;
        d_dataLength         = 0;
        d_preDataIndexLength = 0;
        return;                                                       // RETURN
    }

    // Grow if needed.

    while (length > d_totalSize) {
        BlobBuffer buf;
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
Blob::Blob(bslma::Allocator *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(0)
, d_preDataIndexLength(0)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(0))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(BlobBufferFactory *factory,
           bslma::Allocator  *basicAllocator)
: d_buffers(basicAllocator)
, d_totalSize(0)
, d_dataLength(0)
, d_dataIndex(0)
, d_preDataIndexLength(0)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(factory))
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

Blob::Blob(const Blob&        original,
           BlobBufferFactory *factory,
           bslma::Allocator  *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(factory))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
}

Blob::Blob(const Blob&       original,
           bslma::Allocator *basicAllocator)
: d_buffers(original.d_buffers, basicAllocator)
, d_totalSize(original.d_totalSize)
, d_dataLength(original.d_dataLength)
, d_dataIndex(original.d_dataIndex)
, d_preDataIndexLength(original.d_preDataIndexLength)
, d_bufferFactory_p(InvalidBlobBufferFactory::factory(0))
{
    BSLS_ASSERT_SAFE(0 == assertInvariants());
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

void Blob::appendBuffer(const BlobBuffer& buffer)
{
    d_buffers.push_back(buffer);
    d_totalSize += buffer.size();
}

void Blob::appendDataBuffer(const BlobBuffer& buffer)
{
    const int bufferSize = buffer.size();
    const int oldDataLength = d_dataLength;

    d_totalSize  += bufferSize;
    d_dataLength += bufferSize;

    if (d_totalSize == d_dataLength) {
        // Fast path.  At the start, we had 0 or more buffers in the blob and
        // they were all full.

        BSLS_ASSERT_SAFE(d_dataIndex == (int) d_buffers.size() - 1 ||
                                  (0 == d_dataIndex && 0 == d_buffers.size()));

        d_buffers.push_back(buffer);
        d_preDataIndexLength = oldDataLength;
        d_dataIndex = d_buffers.size() - 1;
    }
    else if (bufferSize == d_dataLength) {
        // Another fast path.  At the start, there was no data, but empty
        // buffers were present.  Put the new buffer at the front.

        BSLS_ASSERT_SAFE(d_totalSize > d_dataLength);
        BSLS_ASSERT_SAFE(0 == d_dataIndex);
        BSLS_ASSERT_SAFE(0 == d_preDataIndexLength);

        d_buffers.insert(d_buffers.begin(), buffer);
    }
    else {
        // Complicated case -- at the start, buffer(s) with data were present,
        // trimming 'prevBuf' might or might not be necessary, empty space was
        // present on the end, whole empty buffer(s) might or might not have
        // been present on the end.

        BSLS_ASSERT_SAFE(d_dataLength > bufferSize);
        BSLS_ASSERT_SAFE(d_dataLength < d_totalSize);
        BSLS_ASSERT_SAFE((unsigned) d_dataIndex < d_buffers.size());
        BSLS_ASSERT_SAFE(oldDataLength >= d_preDataIndexLength);

        BlobBuffer& prevBuf     = d_buffers[d_dataIndex];
        const unsigned newPrevBufSize = oldDataLength - d_preDataIndexLength;
        const unsigned trim           = prevBuf.size() - newPrevBufSize;

        BSLS_ASSERT_SAFE(trim <= (unsigned) prevBuf.size());

        prevBuf.setSize(newPrevBufSize);

        ++d_dataIndex;
        d_buffers.insert(d_buffers.begin() + d_dataIndex, buffer);
        d_preDataIndexLength = oldDataLength;
        d_totalSize -= trim;
    }
}

void Blob::insertBuffer(int index, const BlobBuffer& buffer)
{
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

void Blob::prependDataBuffer(const BlobBuffer& buffer)
{
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

void Blob::removeAll()
{
    d_buffers.clear();
    d_totalSize  = 0;
    d_dataLength = 0;
    d_dataIndex  = 0;
    d_preDataIndexLength = 0;
}

void Blob::removeBuffer(int index)
{
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

void Blob::setLength(int length)
{
    BSLS_ASSERT(0 <= length);

    if (d_totalSize
     && d_preDataIndexLength + d_buffers[d_dataIndex].size() >= length
     && d_preDataIndexLength < length) {
        // We are not crossing any buffer boundaries.

        d_dataLength = length;
        return;                                                       // RETURN
    }

    return slowSetLength(length);
}

void Blob::swapBufferRaw(int index, BlobBuffer *srcBuffer)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < numBuffers());
    BSLS_ASSERT(srcBuffer->size() == buffer(index).size());

    d_buffers[index].buffer().swap(srcBuffer->buffer());
}

void Blob::trimLastDataBuffer()
{
    if (0 != d_dataLength
     && d_dataLength - d_preDataIndexLength < d_buffers[d_dataIndex].size()) {
        d_totalSize -= d_buffers[d_dataIndex].size();
        d_buffers[d_dataIndex].setSize(d_dataLength - d_preDataIndexLength);
        d_totalSize += d_dataLength - d_preDataIndexLength;
    }
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

void Blob::moveAndAppendDataBuffers(Blob *srcBlob)
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

    d_buffers.insert(dstIter, numSrcDataBuffers, BlobBuffer());

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
}  // close package namespace

// FREE OPERATORS
bool btlb::operator==(const Blob& lhs, const Blob& rhs)
{
    return lhs.d_buffers            == rhs.d_buffers
        && lhs.d_totalSize          == rhs.d_totalSize
        && lhs.d_dataLength         == rhs.d_dataLength
        && lhs.d_dataIndex          == rhs.d_dataIndex
        && lhs.d_preDataIndexLength == rhs.d_preDataIndexLength;
}

bool btlb::operator!=(const Blob& lhs, const Blob& rhs)
{
    return lhs.d_buffers            != rhs.d_buffers
        || lhs.d_totalSize          != rhs.d_totalSize
        || lhs.d_dataLength         != rhs.d_dataLength
        || lhs.d_dataIndex          != rhs.d_dataIndex
        || lhs.d_preDataIndexLength != rhs.d_preDataIndexLength;
}

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
