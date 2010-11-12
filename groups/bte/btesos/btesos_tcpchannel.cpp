// btesos_tcpchannel.cpp     -*-C++-*-
#include <btesos_tcpchannel.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcpchannel_cpp,"$Id$ $CSID$")

#include <bteso_streamsocket.h>
#include <bteso_sockethandle.h>
#include <btesc_flag.h>
#include <btes_iovecutil.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_vector.h>

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // Local typedefs and enums
                       // ========================

enum {
    ERROR_INTERRUPTED  =  1,
    ERROR_EOF          = -1,
    ERROR_INVALID      = -2,
    ERROR_UNCLASSIFIED = -3
};

                       // ==============================
                       // local function adjustVecBuffer
                       // ==============================

template <class VECTYPE>
inline
int adjustVecBuffer(const VECTYPE        *buffers,
                    int                  *numBuffers,
                    int                   numBytesExisted,
                    bsl::vector<VECTYPE> *vec)
    // This function is to adjust "btes_IoVec" or "btes_Ovec" 'buffers' given
    // the specified 'numBuffers' and 'numBytesExisted' in the 'buffers', such
    // that return the corresponding new buffers which point to unused space in
    // 'buffers'.  Return the pointer to new buffers.  The result is undefined
    // unless the 'buffers' are valid and 'numBuffers' > 0.
{
    int idx = 0,  offset = 0;
    btes_IovecUtil::pivot(&idx, &offset, buffers,
                          *numBuffers, numBytesExisted);
    BSLS_ASSERT(0 <= idx);
    BSLS_ASSERT(idx < *numBuffers);
    BSLS_ASSERT(0 <= offset);
    BSLS_ASSERT(offset < buffers[idx].length());

    vec->clear();

    vec->push_back(VECTYPE((char*) buffers[idx].buffer() + offset,
                                   buffers[idx].length() - offset));

    for (int i = idx + 1; i < *numBuffers; ++i) {
        vec->push_back(btes_Iovec((char*) buffers[i].buffer(),
                       buffers[i].length()));
    }
    *numBuffers -= idx;

    return idx;
}

// ============================================================================
//                        END LOCAL DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class btesos_TcpChannel
                          // -----------------------

// PRIVATE MANIPULATORS

void btesos_TcpChannel::initializeReadBuffer(int size)
{
    if (size > 0) {
        d_readBuffer.resize(size);
    }
    else {
        enum { DEFAULT_BUFFER_SIZE = 8192 };
        int result;
        int s = d_socket_p->socketOption(
                                     &result,
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                     bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
        if (!s) {
            BSLS_ASSERT(0 < result);
            d_readBuffer.resize(result);
        }
        else {
            d_readBuffer.resize(DEFAULT_BUFFER_SIZE);
        }
    }
}

// CREATORS

btesos_TcpChannel::btesos_TcpChannel(
                    bteso_StreamSocket<bteso_IPv4Address> *socket,
                    bslma_Allocator                       *basicAllocator)
: d_socket_p(socket)
, d_isInvalidFlag(0)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferedStartPointer(0)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(d_socket_p);
    d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
}

btesos_TcpChannel::~btesos_TcpChannel()
{
    invalidate();
}

// MANIPULATORS

///Read section
///------------

int btesos_TcpChannel::read(char *buffer, int numBytes, int flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
            return numBytesRead;
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
                return numBytesRead;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesRead;        // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::read(int  *augStatus,
                            char *buffer,
                            int   numBytes,
                            int   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
            return numBytesRead;
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
                return numBytes;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                return numBytesRead; // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::readv(const btes_Iovec *buffers,
                             int               numBuffers,
                             int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead = 0,
        rc = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    bsl::vector<btes_Iovec> readBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        readBuffers.push_back(btes_Iovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btes_IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;
        }
        else {
            numBytesRead = availableData;
            btes_IovecUtil::scatter(buffers,
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
                return numBytesRead;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesRead,
                                &readBuffers);
                btes_IovecUtil::length(&readBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesRead;  // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::readv(int              *augStatus,
                             const btes_Iovec *buffers,
                             int               numBuffers,
                             int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead = 0,
        rc = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    bsl::vector<btes_Iovec> readBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        readBuffers.push_back(btes_Iovec(buffers[i].buffer(),
                              buffers[i].length()));
    }

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btes_IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            if (length < availableData) {
                d_readBufferedStartPointer += length;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
            return numBytesRead;
        }
        else {
            numBytesRead = availableData;
            btes_IovecUtil::scatter(buffers,
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
                return numBytesRead;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesRead,
                                &readBuffers);
                btes_IovecUtil::length(&readBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                return numBytesRead;  // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::readRaw(char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
        return numBytesRead;
    }

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {// EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::readRaw(int  *,
                               char *buffer,
                               int   numBytes,
                               int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
        return numBytesRead;
    }

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::readvRaw(const btes_Iovec *buffers,
                                int               numBuffers,
                                int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        length = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;
    const btes_Iovec *readBuffers = buffers;

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btes_IovecUtil::scatter(buffers,
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
            btes_IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {              // 'length' is expected to be read back.
        rc = d_socket_p->readv(readBuffers, numBuffers);
        if (0 < rc) {        // This read operation got some bytes back.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::readvRaw(int              *,
                                const btes_Iovec *buffers,
                                int               numBuffers,
                                int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, numBytesRead = 0, retValue = 0,
        length = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;
    const btes_Iovec *readBuffers = buffers;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    if (availableData) {
        if (length <= availableData) {
            numBytesRead = length;
            btes_IovecUtil::scatter(buffers,
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
            btes_IovecUtil::scatter(buffers,
                                    numBuffers,
                                    &d_readBuffer[d_readBufferedStartPointer],
                                    numBytesRead);
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    while (1) {              // 'length' is expected to be read back.
        rc = d_socket_p->readv(readBuffers, numBuffers);

        if (0 < rc) {        // This read operation got some bytes back.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::bufferedRead(const char **buffer,
                                    int          numBytes,
                                    int          flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
            return numBytesRead;
        }
        else {
            numBytesRead = availableData;
            // Move the unconsumed data at the beginning of the internal
            // buffer and try reading from the channel to 'd_readBuffer'
            // after these data.
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
                return numBytes;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {   // interruptible
                                                               // mode
                *buffer = 0;       // not returned
                d_readBufferOffset = numBytesRead;
                return numBytesRead; // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::bufferedRead(int         *augStatus,
                                    const char **buffer,
                                    int          numBytes,
                                    int          flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
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
            return numBytesRead;
        }
        else {
            numBytesRead = availableData;
            // Move the unconsumed data at the beginning of the internal
            // buffer and try reading from the channel to 'd_readBuffer'
            // after these data.
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
                return numBytes;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                *buffer = 0;
                d_readBufferOffset = numBytesRead;
                return numBytesRead; // Return the total bytes read.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "EOF" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesRead;
}

int btesos_TcpChannel::bufferedReadRaw(const char **buffer,
                                       int          numBytes,
                                       int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc            = 0;
    int numBytesRead  = 0;
    int retValue      = 0;
    int availableData = d_readBufferOffset - d_readBufferedStartPointer;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        return numBytesRead;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::bufferedReadRaw(int         *,
                                       const char **buffer,
                                       int          numBytes,
                                       int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    // BSLS_ASSERT(numBytes <= d_readBuffer.size());
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc            = 0;
    int numBytesRead  = 0;
    int retValue      = 0;
    int availableData = d_readBufferOffset - d_readBufferedStartPointer;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        return numBytesRead;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "EOF" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

///Write section
///-------------

int btesos_TcpChannel::write(const char *buffer,
                             int         numBytes,
                             int         flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, numBytesWritten = 0;
    while (numBytesWritten < numBytes) {
        errno = 0;
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);

        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) { // Read 'numBytes' successfully.
                return numBytesWritten;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesWritten; // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::write(int        *augStatus,
                             const char *buffer,
                             int         numBytes,
                             int         flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, numBytesWritten = 0;
    while (numBytesWritten < numBytes) {
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);

        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) { // Read 'numBytes' successfully.
                return numBytesWritten;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                return numBytesWritten; // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::writeRaw(const char *buffer,
                                int         numBytes,
                                int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;
    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::writeRaw(int        *,
                                const char *buffer,
                                int         numBytes,
                                int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;
    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::writev(const btes_Ovec  *buffers,
                              int               numBuffers,
                              int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers);

    bsl::vector<btes_Ovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btes_Ovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btes_IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesWritten;  // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::writev(const btes_Iovec *buffers,
                              int               numBuffers,
                              int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers);

    bsl::vector<btes_Iovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btes_Iovec(buffers[i].buffer(),
                                          buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btes_IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                return numBytesWritten;  // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::writev(int              *augStatus,
                              const btes_Ovec  *buffers,
                              int               numBuffers,
                              int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers);

    bsl::vector<btes_Ovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btes_Ovec(buffers[i].buffer(),
                                         buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btes_IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) { // interruptible
                *augStatus = ERROR_INTERRUPTED;
                return numBytesWritten;  // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::writev(int              *augStatus,
                              const btes_Iovec *buffers,
                              int               numBuffers,
                              int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0,
        numBytesWritten = 0,
        originNumBuffers = numBuffers,
        length = btes_IovecUtil::length(buffers, numBuffers);

    bsl::vector<btes_Iovec> writeBuffers(d_allocator_p);
    for (int i = 0; i < numBuffers; ++i){
        writeBuffers.push_back(btes_Iovec(buffers[i].buffer(),
                                          buffers[i].length()));
    }

    while (numBytesWritten < length) {
        rc = d_socket_p->writev(&writeBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesWritten += rc;
            if (length == numBytesWritten) { // This write operation succeeded.
                return numBytesWritten;
            }
            else {
                // Adjust the buffer for next "read" try.
                numBuffers = originNumBuffers;
                adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesWritten,
                                &writeBuffers);
                btes_IovecUtil::length(&writeBuffers.front(), numBuffers);
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                return numBytesWritten;  // Return the total bytes written.
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            return ERROR_EOF;
        }
        else {
            // Errors other than "asynchronous event" or "" occur.
            d_isInvalidFlag = 1;
            return ERROR_UNCLASSIFIED;
        }
    }
    return numBytesWritten;
}

int btesos_TcpChannel::writevRaw(const btes_Ovec *buffers,
                                 int              numBuffers,
                                 int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::writevRaw(const btes_Iovec *buffers,
                                 int               numBuffers,
                                 int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {              // 'length' is expected to be written.
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::writevRaw(int             *,
                                 const btes_Ovec *buffers,
                                 int              numBuffers,
                                 int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

int btesos_TcpChannel::writevRaw(int              *,
                                 const btes_Iovec *buffers,
                                 int               numBuffers,
                                 int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = 0, retValue = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {        // This read operation wrote some bytes.
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else { // Errors other than "AE" or "CONNDEAD" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }
    if (0 == d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }
    return retValue;
}

// ACCESSORS

int btesos_TcpChannel::getLocalAddress(bteso_IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->localAddress(result);
}

int btesos_TcpChannel::getOption(int *result, int level, int option)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->socketOption(result, level, option);
}

int btesos_TcpChannel::getPeerAddress(bteso_IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->peerAddress(result);
}

int btesos_TcpChannel::setOption(int level, int option, int value)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->setOption(level, option, value);
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
