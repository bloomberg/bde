// btlsos_tcpchannel.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcpchannel.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcpchannel_cpp,"$Id$ $CSID$")

#include <btlso_streamsocket.h>
#include <btlso_sockethandle.h>
#include <btlsc_flag.h>
#include <btls_iovecutil.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_vector.h>

#include <errno.h>

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                     // ===============================
                     // Local typedefs and enumerations
                     // ===============================

enum {
    e_ERROR_INTERRUPTED  =  1,
    e_ERROR_EOF          = -1,
    e_ERROR_INVALID      = -2,
    e_ERROR_UNCLASSIFIED = -3
};

                      // ==============================
                      // local function adjustVecBuffer
                      // ==============================

template <class VECTYPE>
inline
int adjustVecBuffer(const VECTYPE        *buffers,
                    int                  *numBuffers,
                    int                   numBytesExisted,
                    bsl::vector<VECTYPE> *vector)
    // This function is to adjust the specified 'buffer', whether "btes::IoVec"
    // or "btls::Ovec", given the specified 'numBuffers' and 'numBytesExisted'
    // in the 'buffers', such that return the corresponding new buffers which
    // point to unused space in 'buffers'.  Return the pointer to new buffers.
    // The result is undefined unless the 'buffers' are valid and
    // 'numBuffers > 0'.
{
    int idx = 0,  offset = 0;
    btls::IovecUtil::pivot(&idx, &offset, buffers,
                          *numBuffers, numBytesExisted);
    BSLS_ASSERT(0 <= idx);
    BSLS_ASSERT(idx < *numBuffers);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset < buffers[idx].length());

    vector->clear();

    vector->push_back(VECTYPE(
                (char*) const_cast<void *>(buffers[idx].buffer()) + offset,
                buffers[idx].length() - offset));

    for (int i = idx + 1; i < *numBuffers; ++i) {
        vector->push_back(
                btls::Iovec((char*) const_cast<void *>(buffers[i].buffer()),
                           buffers[i].length()));
    }
    *numBuffers -= idx;

    return idx;
}

namespace btlsos {

// ============================================================================
//                           END LOCAL DEFINITIONS
// ============================================================================

                             // ----------------
                             // class TcpChannel
                             // ----------------

// PRIVATE MANIPULATORS

void TcpChannel::initializeReadBuffer(int size)
{
    if (size > 0) {
        d_readBuffer.resize(size);
    }
    else {
        enum { k_DEFAULT_BUFFER_SIZE = 8192 };
        int result;
        int s = d_socket_p->socketOption(
                                    &result,
                                    btlso::SocketOptUtil::k_SOCKETLEVEL,
                                    btlso::SocketOptUtil::k_RECEIVEBUFFER);
        if (!s) {
            BSLS_ASSERT(0 < result);
            d_readBuffer.resize(result);
        }
        else {
            d_readBuffer.resize(k_DEFAULT_BUFFER_SIZE);
        }
    }
}

// CREATORS

TcpChannel::TcpChannel(btlso::StreamSocket<btlso::IPv4Address> *socket,
                       bslma::Allocator                        *basicAllocator)
: d_socket_p(socket)
, d_isInvalidFlag(0)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferedStartPointer(0)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(d_socket_p);
    d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
}

TcpChannel::~TcpChannel()
{
    invalidate();
}

// MANIPULATORS

///Read section
///------------

int TcpChannel::read(char *buffer, int numBytes, int flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int numBytesRead  = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
    }

    while (numBytesRead < numBytes) {
        int rc = d_socket_p->read(buffer + numBytesRead,
                                  numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                return numBytesRead;                                  // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                // Return the total bytes read.

                return numBytesRead;                                  // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::read(int *augStatus, char *buffer, int numBytes, int flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int numBytesRead  = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
    }

    while (numBytesRead < numBytes) {
        int rc = d_socket_p->read(buffer + numBytesRead,
                                  numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;    // Keep a record of the total bytes read.
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                return numBytes;                                      // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = e_ERROR_INTERRUPTED;
                return numBytesRead; // Return the total bytes read.  // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::readv(const btls::Iovec *buffers, int numBuffers, int flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int numBytesRead = 0,
        rc = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    bsl::vector<btls::Iovec> readBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        readBuffers.push_back(btls::Iovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    availableData);

            d_readBufferedStartPointer = d_readBufferOffset = 0;
            // Adjust the buffer for next "read" try.
            adjustVecBuffer(buffers, &numBuffers, numBytesRead, &readBuffers);
        }
    }

    while (numBytesRead < length) {
        rc = d_socket_p->readv(&readBuffers.front(), numBuffers);

        if (0 < rc) {           // This read operation got some bytes back.
            numBytesRead += rc;
            if (length == numBytesRead) {  // This read operation succeeded.
                return numBytesRead;                                  // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesRead,
                                &readBuffers);
                btls::IovecUtil::length(&readBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesRead;  // Return the total bytes read. // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::readv(int               *augStatus,
                      const btls::Iovec *buffers,
                      int                numBuffers,
                      int                flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int numBytesRead = 0,
        rc = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    bsl::vector<btls::Iovec> readBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        readBuffers.push_back(btls::Iovec(buffers[i].buffer(),
                              buffers[i].length()));
    }

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    availableData);

            d_readBufferedStartPointer = d_readBufferOffset = 0;
            // Adjust the buffer for next "read" try.
            adjustVecBuffer(buffers, &numBuffers, numBytesRead, &readBuffers);
        }
    }

    while (numBytesRead < length) {
        rc = d_socket_p->readv(&readBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesRead += rc;
            if (length == numBytesRead) {  // This read operation succeeded.
                return numBytesRead;                                  // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesRead,
                                &readBuffers);
                btls::IovecUtil::length(&readBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = e_ERROR_INTERRUPTED;
                return numBytesRead;  // Return the total bytes read. // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::readRaw(char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {// EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::readRaw(int *, char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            bsl::memcpy(buffer,
                        &d_readBuffer[d_readBufferedStartPointer],
                        numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::readvRaw(const btls::Iovec *buffers, int numBuffers, int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        length = btls::IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;
    const btls::Iovec *readBuffers = buffers;

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {              // 'length' is expected to be read back.
        rc = d_socket_p->readv(readBuffers, numBuffers);
        if (0 < rc) {        // This read operation got some bytes back.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::readvRaw(int               *,
                         const btls::Iovec *buffers,
                         int                numBuffers,
                         int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        length = btls::IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;
    const btls::Iovec *readBuffers = buffers;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                              numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            btls::IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    while (1) {              // 'length' is expected to be read back.
        rc = d_socket_p->readv(readBuffers, numBuffers);

        if (0 < rc) {        // This read operation got some bytes back.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::bufferedRead(const char **buffer, int numBytes, int flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }
    if (0 == d_readBuffer.size()) {
        initializeReadBuffer();
    }
    if ((int) d_readBuffer.size() < numBytes) {
        d_readBuffer.resize(numBytes);
    }

    int rc = 0, numBytesRead = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            // Move the unconsumed data at the beginning of the internal buffer
            // and try reading from the channel to 'd_readBuffer' after these
            // data.
            bsl::memcpy(&d_readBuffer.front(),
                        &d_readBuffer[d_readBufferedStartPointer],
                        availableData);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
    }

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(&d_readBuffer.front() + numBytesRead,
                              numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;      // Keep a record of the total bytes read.
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                *buffer = &d_readBuffer.front();
                return numBytes;                                      // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {   // interruptible
                                                               // mode
                *buffer = 0;       // not returned
                d_readBufferOffset = numBytesRead;
                return numBytesRead; // Return the total bytes read.  // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::bufferedRead(int         *augStatus,
                             const char **buffer,
                             int          numBytes,
                             int          flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }
    if ((int) d_readBuffer.size() < numBytes) {
        d_readBuffer.resize(numBytes);
    }

    int rc = 0, numBytesRead = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;                                      // RETURN
        }
        else {
            numBytesRead = availableData;
            // Move the unconsumed data at the beginning of the internal buffer
            // and try reading from the channel to 'd_readBuffer' after these
            // data.
            bsl::memcpy(&d_readBuffer.front(),
                        &d_readBuffer[d_readBufferedStartPointer],
                        availableData);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
    }

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(&d_readBuffer.front() + numBytesRead,
                              numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;     // Keep a record of the total bytes read.
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                *buffer = &d_readBuffer.front();
                return numBytes;                                      // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = e_ERROR_INTERRUPTED;
                *buffer = 0;
                d_readBufferOffset = numBytesRead;
                return numBytesRead; // Return the total bytes read.  // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesRead;
}

int TcpChannel::bufferedReadRaw(const char **buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc            = 0;
    int numBytesRead  = 0;
    int retValue      = 0;
    int availableData = d_readBufferOffset - d_readBufferedStartPointer;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    BSLS_ASSERT(0 == d_readBufferedStartPointer);
    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    while (1) {
        rc = d_socket_p->read(&d_readBuffer.front(), numBytes);
        if (0 < rc) {
            *buffer = &d_readBuffer.front();
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::bufferedReadRaw(int *, const char **buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    // BSLS_ASSERT(numBytes <= d_readBuffer.size());
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc            = 0;
    int numBytesRead  = 0;
    int retValue      = 0;
    int availableData = d_readBufferOffset - d_readBufferedStartPointer;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    if (availableData) {
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];

            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;                                          // RETURN
    }

    BSLS_ASSERT(0 == d_readBufferedStartPointer);
    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    while (1) {
        rc = d_socket_p->read(&d_readBuffer.front(), numBytes);
        if (0 < rc) {
            *buffer = &d_readBuffer.front();
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_READ);

            if (btlso::Flag::e_IO_READ == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

///Write section
///-------------

int TcpChannel::write(const char *buffer, int numBytes, int flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesWritten = 0;
    while (numBytesWritten < numBytes) {
        errno = 0;
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);

        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) { // Read 'numBytes' successfully.
                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::write(int        *augStatus,
                      const char *buffer,
                      int         numBytes,
                      int         flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, numBytesWritten = 0;
    while (numBytesWritten < numBytes) {
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);

        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) { // Read 'numBytes' successfully.
                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                *augStatus = e_ERROR_INTERRUPTED;

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::writeRaw(const char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;
    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::writeRaw(int *, const char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;
    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::writev(const btls::Ovec *buffers, int numBuffers, int flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers);

    bsl::vector<btls::Ovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btls::Ovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;                               // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btls::IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::writev(const btls::Iovec *buffers, int numBuffers, int flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers);

    bsl::vector<btls::Iovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btls::Iovec(buffers[i].buffer(),
                                          buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;                               // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btls::IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::writev(int              *augStatus,
                       const btls::Ovec *buffers,
                       int               numBuffers,
                       int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers);

    bsl::vector<btls::Ovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btls::Ovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;                               // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btls::IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible

                *augStatus = e_ERROR_INTERRUPTED;

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::writev(int               *augStatus,
                       const btls::Iovec *buffers,
                       int                numBuffers,
                       int                flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btls::IovecUtil::length(buffers, numBuffers);

    bsl::vector<btls::Iovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btls::Iovec(buffers[i].buffer(),
                                          buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;                               // RETURN
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btls::IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                // interruptible mode

                *augStatus = e_ERROR_INTERRUPTED;

                // Return the total bytes written.

                return numBytesWritten;                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return e_ERROR_EOF;                                       // RETURN
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return e_ERROR_UNCLASSIFIED;                              // RETURN
        }
    }
    return numBytesWritten;
}

int TcpChannel::writevRaw(const btls::Ovec *buffers, int numBuffers, int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::writevRaw(const btls::Iovec *buffers, int numBuffers, int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {              // 'length' is expected to be written.
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::writevRaw(int              *,
                          const btls::Ovec *buffers,
                          int               numBuffers,
                          int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int TcpChannel::writevRaw(int               *,
                          const btls::Iovec *buffers,
                          int                numBuffers,
                          int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return e_ERROR_INVALID;                                       // RETURN
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(btlso::Flag::e_IO_WRITE);

            if (btlso::Flag::e_IO_WRITE == rc ||
                btlso::SocketHandle::e_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (btlso::SocketHandle::e_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = e_ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = e_ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

// ACCESSORS

int TcpChannel::getLocalAddress(btlso::IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->localAddress(result);
}

int TcpChannel::getOption(int *result, int level, int option)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->socketOption(result, level, option);
}

int TcpChannel::getPeerAddress(btlso::IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->peerAddress(result);
}

int TcpChannel::setOption(int level, int option, int value)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->setOption(level, option, value);
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
