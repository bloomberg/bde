// btesos_tcptimedchannel.cpp     -*-C++-*-
#include <btesos_tcptimedchannel.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcptimedchannel_cpp,"$Id$ $CSID$")

#include <bteso_streamsocket.h>
#include <bteso_sockethandle.h>
#include <btesc_flag.h>
#include <btes_iovecutil.h>

#include <bslma_default.h>
#include <bdet_timeinterval.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_vector.h>

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // local typedefs and enums
                       // ========================

enum {
    ERROR_INTERRUPTED  =  1,
    ERR_TIMEOUT        =  0,
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
//                        END OF LOCAL DEFINITIONS
// ============================================================================

                          // ----------------------------
                          // class btesos_TcpTimedChannel
                          // ----------------------------

// PRIVATE MANIPULATORS

void btesos_TcpTimedChannel::initializeReadBuffer(int size)
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

btesos_TcpTimedChannel::btesos_TcpTimedChannel(
                    bteso_StreamSocket<bteso_IPv4Address> *socket,
                    bslma_Allocator                       *basicAllocator)
: d_socket_p(socket)
, d_isInvalidFlag(0)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferedStartPointer(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_socket_p);
    d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
}

btesos_TcpTimedChannel::~btesos_TcpTimedChannel()
{
    invalidate();
}

// MANIPULATORS

///Read section
///------------

int btesos_TcpTimedChannel::read(char *buffer, int numBytes, int flags)
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

int btesos_TcpTimedChannel::read(int  *augStatus,
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

int btesos_TcpTimedChannel::timedRead(char                     *buffer,
                                      int                       numBytes,
                                      const bdet_TimeInterval&  timeout,
                                      int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(buffer + numBytesRead, numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                retValue = numBytesRead;
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF" / "TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedRead(int                      *augStatus,
                                      char                     *buffer,
                                      int                       numBytes,
                                      const bdet_TimeInterval&  timeout,
                                      int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(buffer + numBytesRead, numBytes - numBytesRead);

        if (0 < rc) {
            numBytesRead += rc;
            if (numBytes == numBytesRead) {
                retValue = numBytesRead;
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);
            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }

        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                *augStatus = ERROR_INTERRUPTED;
                break;
            }
        }
        else if (rc < 0) {
            // Errors other than "AE", "EOF" or "TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::readv(const btes_Iovec *buffers,
                                  int               numBuffers,
                                  int               flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead     = 0,
        originNumBuffers = numBuffers,
        length           = btes_IovecUtil::length(buffers, numBuffers),
        availableData    = d_readBufferOffset - d_readBufferedStartPointer;

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
            adjustVecBuffer(buffers,
                            &numBuffers,
                            numBytesRead, &readBuffers);
        }
    }

    while (numBytesRead < length) {
        int rc = d_socket_p->readv(&readBuffers.front(), numBuffers);

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

int btesos_TcpTimedChannel::readv(int              *augStatus,
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

    int numBytesRead     = 0,
        originNumBuffers = numBuffers,
        length           = btes_IovecUtil::length(buffers, numBuffers),
        availableData    = d_readBufferOffset - d_readBufferedStartPointer;

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
            adjustVecBuffer(buffers,
                            &numBuffers,
                            numBytesRead,
                            &readBuffers);
        }
    }

    while (numBytesRead < length) {
        int rc = d_socket_p->readv(&readBuffers.front(), numBuffers);

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

int btesos_TcpTimedChannel::timedReadv(const btes_Iovec         *buffers,
                                       int                       numBuffers,
                                       const bdet_TimeInterval&  timeout,
                                       int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead     = 0,
        retValue         = 0,
        originNumBuffers = numBuffers,
        length           = btes_IovecUtil::length(buffers, numBuffers),
        availableData    = d_readBufferOffset - d_readBufferedStartPointer;

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
            adjustVecBuffer(buffers,
                            &numBuffers,
                            numBytesRead,
                            &readBuffers);
        }
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesRead < length) {
        rc = d_socket_p->readv(&readBuffers.front(), numBuffers);

        if (0 < rc) {
            numBytesRead += rc;
            if (length == numBytesRead) { // Read 'numBytes' successfully.
                retValue = numBytesRead;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occurs.
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

int btesos_TcpTimedChannel::timedReadv(int                      *augStatus,
                                       const btes_Iovec         *buffers,
                                       int                       numBuffers,
                                       const bdet_TimeInterval&  timeout,
                                       int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead     = 0,
        retValue         = 0,
        originNumBuffers = numBuffers,
        length           = btes_IovecUtil::length(buffers, numBuffers),
        availableData    = d_readBufferOffset - d_readBufferedStartPointer;

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
            adjustVecBuffer(buffers,
                                &numBuffers,
                                numBytesRead,
                                &readBuffers);
        }
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesRead < length) {
        rc = d_socket_p->readv(&readBuffers.front(), numBuffers);
        if (0 < rc) {
            numBytesRead += rc;
            if (length == numBytesRead) {
                retValue = numBytesRead;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                *augStatus = ERROR_INTERRUPTED;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occurs.
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

int btesos_TcpTimedChannel::readRaw(char *buffer, int numBytes, int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        else if (rc < 0) { // Errors other than "AE" or "EOF" occur.
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

int btesos_TcpTimedChannel::readRaw(int  *,
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

    int numBytesRead  = 0,
        retValue      = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        else if (rc < 0) { // Errors other than "AE" or "EOF" occur.
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

int btesos_TcpTimedChannel::timedReadRaw(char                     *buffer,
                                         int                       numBytes,
                                         const bdet_TimeInterval&  timeout,
                                         int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
        rc            = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

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

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/ "TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedReadRaw(int                      *augStatus,
                                         char                     *buffer,
                                         int                       numBytes,
                                         const bdet_TimeInterval&  timeout,
                                         int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::readvRaw(const btes_Iovec *buffers,
                                     int               numBuffers,
                                     int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
        length        = btes_IovecUtil::length(buffers, numBuffers),
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        else if (rc < 0) { // Errors other than "AE" or "EOF" occur.
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

int btesos_TcpTimedChannel::readvRaw(int              *,
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

    int numBytesRead  = 0,
        retValue      = 0,
        length        = btes_IovecUtil::length(buffers, numBuffers),
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
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
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {     // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (rc < 0) { // Errors other than "AE" or "EOF" occur.
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

int btesos_TcpTimedChannel::timedReadvRaw(const btes_Iovec        *buffers,
                                          int                      numBuffers,
                                          const bdet_TimeInterval& timeout,
                                          int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
        length        = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->readv(buffers, numBuffers);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedReadvRaw(int                     *augStatus,
                                          const btes_Iovec        *buffers,
                                          int                      numBuffers,
                                          const bdet_TimeInterval& timeout,
                                          int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
        length        = btes_IovecUtil::length(buffers, numBuffers),
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->readv(buffers, numBuffers);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::bufferedRead(const char **buffer,
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

    int numBytesRead  = 0,
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
        int rc = d_socket_p->read(&d_readBuffer[numBytesRead],
                                  numBytes - numBytesRead);
        if (0 < rc) {
            numBytesRead += rc;      // Keep a record of the total bytes read.
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                *buffer = &d_readBuffer.front();
                return numBytes;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
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

int btesos_TcpTimedChannel::bufferedRead(int         *augStatus,
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

    int numBytesRead  = 0,
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

    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    while (numBytesRead < numBytes) {
        int rc = d_socket_p->read(&d_readBuffer[numBytesRead],
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

int btesos_TcpTimedChannel::timedBufferedRead(
                                           const char              **buffer,
                                           int                       numBytes,
                                           const bdet_TimeInterval&  timeout,
                                           int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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

    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(&d_readBuffer[numBytesRead],
                              numBytes - numBytesRead);
        if (0 < rc) {
            numBytesRead += rc;
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                retValue = numBytesRead;
                *buffer = &d_readBuffer.front();
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);
            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            *buffer = 0;
            d_readBufferOffset = numBytesRead;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                *buffer = 0;
                d_readBufferOffset = numBytesRead;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/ "TIMEDOUT" occur.
            d_isInvalidFlag = 1;
            retValue = ERROR_UNCLASSIFIED;
            break;
        }
    }

    if (d_isInvalidFlag) {
        rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
        BSLS_ASSERT(0 == rc);
    }

    return retValue;
}

int btesos_TcpTimedChannel::timedBufferedRead(
                                           int                      *augStatus,
                                           const char              **buffer,
                                           int                       numBytes,
                                           const bdet_TimeInterval&  timeout,
                                           int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    while (numBytesRead < numBytes) {
        rc = d_socket_p->read(&d_readBuffer[numBytesRead],
                              numBytes - numBytesRead);
        if (0 < rc) {
            numBytesRead += rc;
            if (numBytes == numBytesRead) { // Read 'numBytes' successfully.
                retValue = numBytesRead;
                *buffer = &d_readBuffer.front();
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc) {
                continue;
            }
        }

        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesRead;
            *augStatus = ERR_TIMEOUT;
            *buffer = 0;
            d_readBufferOffset = numBytesRead;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesRead;
                *augStatus = ERROR_INTERRUPTED;
                *buffer = 0;
                d_readBufferOffset = numBytesRead;
                break;
            }
        }
        else if(rc < 0) { // Errors other than "AE", "EOF" or "TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::bufferedReadRaw(const char **buffer,
                                            int          numBytes,
                                            int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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
        }
        else {
            numBytesRead = availableData;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

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

int btesos_TcpTimedChannel::bufferedReadRaw(int         *,
                                            const char **buffer,
                                            int          numBytes,
                                            int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesRead  = 0,
        retValue      = 0,
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
        }
        else {
            numBytesRead = availableData;
            *buffer = &d_readBuffer[d_readBufferedStartPointer];
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

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

int btesos_TcpTimedChannel::timedBufferedReadRaw(
                                           const char              **buffer,
                                           int                       numBytes,
                                           const bdet_TimeInterval&  timeout,
                                           int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        *buffer = &d_readBuffer[d_readBufferedStartPointer];
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    BSLS_ASSERT(0 == d_readBufferedStartPointer);
    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    while (1) {
        rc = d_socket_p->read(&d_readBuffer.front(), numBytes);
        if (0 < rc) {
            retValue = rc;
            *buffer = &d_readBuffer.front();
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedBufferedReadRaw(
                                           int                      *augStatus,
                                           const char              **buffer,
                                           int                       numBytes,
                                           const bdet_TimeInterval&  timeout,
                                           int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);
    BSLS_ASSERT(d_readBufferedStartPointer <= d_readBufferOffset);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue      = 0,
        numBytesRead  = 0,
        availableData = d_readBufferOffset - d_readBufferedStartPointer;

    if (availableData) {
        *buffer = &d_readBuffer[d_readBufferedStartPointer];
        if (numBytes <= availableData) {
            numBytesRead = numBytes;
            if (numBytes < availableData) {
                d_readBufferedStartPointer += numBytes;
            }
            else {
                d_readBufferedStartPointer = d_readBufferOffset = 0;
            }
        }
        else {
            numBytesRead = availableData;
            d_readBufferedStartPointer = d_readBufferOffset = 0;
        }
        return numBytesRead;
    }

    BSLS_ASSERT(0 == d_readBufferedStartPointer);
    if (numBytes > (int) d_readBuffer.size()) {
        d_readBuffer.resize(numBytes);
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->read(&d_readBuffer.front(), numBytes);
        if (0 < rc) {
            retValue = rc;
            *buffer = &d_readBuffer.front();
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_READ, timeout);

            if (bteso_Flag::BTESO_IO_READ == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_EOF == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            // The "raw" operation could not complete.
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                *augStatus = ERROR_INTERRUPTED;
                *buffer = &d_readBuffer.front();
                break;
            }
        }
        else if (rc < 0) {  // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::write(const char *buffer,
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
        //bsl::cout << "In write(no-aug): numBytes = " << numBytes
        //          << "; rc = " << rc << "; numBytesWritten = "
        //          << numBytesWritten << bsl::endl;

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

int btesos_TcpTimedChannel::write(int        *augStatus,
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

        //bsl::cout << "In write(aug): numBytes = " << numBytes
        //          << "; rc = " << rc << bsl::endl;

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

int btesos_TcpTimedChannel::timedWrite(const char               *buffer,
                                       int                       numBytes,
                                       const bdet_TimeInterval&  timeout,
                                       int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesWritten = 0, retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesWritten < numBytes) {
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);

        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) { // Read 'numBytes' successfully.
                retValue = numBytesWritten;
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "TW(no-aug) after waitForIo, rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWrite(int                      *augStatus,
                                       const char               *buffer,
                                       int                       numBytes,
                                       const bdet_TimeInterval&  timeout,
                                       int                       flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int numBytesWritten = 0, retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (numBytesWritten < numBytes) {
        rc = d_socket_p->write(buffer + numBytesWritten,
                               numBytes - numBytesWritten);
        if (0 < rc) {
            numBytesWritten += rc;
            if (numBytes == numBytesWritten) {
                retValue = numBytesWritten;
                break;
            }
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "TW(aug) after waitForIo, rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                *augStatus = ERROR_INTERRUPTED;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::writeRaw(const char *buffer,
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
        //bsl::cout << "WR(no-aug), rc = " << rc << bsl::endl;
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
        //bsl::cout << "WR(no-aug) after waitForIo, rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::writeRaw(int        *,
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
        //bsl::cout << "WR(aug), rc = " << rc << bsl::endl;
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
        //bsl::cout << "WR(aug) after waitForIo, rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {
            // The connection is down.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::timedWriteRaw(const char               *buffer,
                                          int                       numBytes,
                                          const bdet_TimeInterval&  timeout,
                                          int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWriteRaw(int                      *augStatus,
                                          const char               *buffer,
                                          int                       numBytes,
                                          const bdet_TimeInterval&  timeout,
                                          int)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    while (1) {
        rc = d_socket_p->write(buffer, numBytes);
        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::writev(const btes_Ovec  *buffers,
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
        //bsl::cout << "In writevo(no-aug), rc = " << rc << bsl::endl;
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

int btesos_TcpTimedChannel::writev(const btes_Iovec *buffers,
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
        //bsl::cout << "In writevi(no-aug), rc = " << rc << bsl::endl;

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

int btesos_TcpTimedChannel::writev(int              *augStatus,
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

int btesos_TcpTimedChannel::writev(int              *augStatus,
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

int btesos_TcpTimedChannel::timedWritev(const btes_Ovec          *buffers,
                                        int                       numBuffers,
                                        const bdet_TimeInterval&  timeout,
                                        int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    int numBytesWritten = 0,
        retValue = 0,
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
            if (numBytesWritten == length) {
                retValue = numBytesWritten;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "In timedwritevo(no-aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritev(const btes_Iovec         *buffers,
                                        int                       numBuffers,
                                        const bdet_TimeInterval&  timeout,
                                        int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    int numBytesWritten = 0,
        retValue = 0,
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
            if (numBytesWritten == length) {
                retValue = numBytesWritten;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "In timedwritevi(no-aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritev(int                      *augStatus,
                                        const btes_Ovec          *buffers,
                                        int                       numBuffers,
                                        const bdet_TimeInterval&  timeout,
                                        int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    int numBytesWritten = 0,
        retValue = 0,
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
            if (numBytesWritten == length) { // This write operation succeeded.
                retValue = numBytesWritten;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "In timedwritevo(aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                *augStatus = ERROR_INTERRUPTED;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritev(int                      *augStatus,
                                        const btes_Iovec         *buffers,
                                        int                       numBuffers,
                                        const bdet_TimeInterval&  timeout,
                                        int                       flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    int numBytesWritten = 0,
        retValue = 0,
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
            if (numBytesWritten == length) {
                retValue = numBytesWritten;
                break;
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
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc) {
                continue;
            }
        }
        //bsl::cout << "In timedwritevi(aug), rc = " << rc << bsl::endl;

        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {    // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            retValue = numBytesWritten;
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {  // interruptible
                                                              // mode
                retValue = numBytesWritten;
                *augStatus = ERROR_INTERRUPTED;
                break;
            }
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::writevRaw(const btes_Ovec *buffers,
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
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);
        //bsl::cout << "In writeviRaw(no-aug), rc = " << rc << bsl::endl;

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
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::writevRaw(const btes_Iovec *buffers,
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
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {              // 'length' is expected to be written.
        rc = d_socket_p->writev(buffers, numBuffers);
        //bsl::cout << "In writeviRaw(no-aug), rc = " << rc << bsl::endl;

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
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::writevRaw(int             *,
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
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);
        //bsl::cout << "In writevoRaw(aug), rc = " << rc << bsl::endl;

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
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::writevRaw(int              *,
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
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);
        // bsl::cout << "In writeviRaw(aug), rc = " << rc << bsl::endl;
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
        else if (rc < 0) { // Errors other than "AE" or "CONNDEAD" occur.
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

int btesos_TcpTimedChannel::timedWritevRaw(const btes_Ovec         *buffers,
                                           int                      numBuffers,
                                           const bdet_TimeInterval& timeout,
                                           int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        //bsl::cout <<"In timedwraw(o: no-aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritevRaw(const btes_Iovec        *buffers,
                                           int                      numBuffers,
                                           const bdet_TimeInterval& timeout,
                                           int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;
    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        //bsl::cout <<"In timedwraw(i: no-aug), rc = "<< rc  << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritevRaw(int                     *augStatus,
                                           const btes_Ovec         *buffers,
                                           int                      numBuffers,
                                           const bdet_TimeInterval& timeout,
                                           int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;

    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        //bsl::cout <<"In timedwraw(o: aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::timedWritevRaw(int                     *augStatus,
                                           const btes_Iovec        *buffers,
                                           int                      numBuffers,
                                           const bdet_TimeInterval& timeout,
                                           int)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidFlag) {
        return ERROR_INVALID;
    }

    int retValue = 0, rc = 0;
    rc = d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (0 != rc) {
        return ERROR_UNCLASSIFIED;
    }

    while (1) {
        rc = d_socket_p->writev(buffers, numBuffers);

        if (0 < rc) {
            retValue = rc;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            rc = d_socket_p->waitForIO(bteso_Flag::BTESO_IO_WRITE, timeout);

            if (bteso_Flag::BTESO_IO_WRITE == rc ||
                bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == rc) {
                continue;
            }
        }
        //bsl::cout <<"In timedwraw(I: aug), rc = " << rc << bsl::endl;
        if (bteso_SocketHandle::BTESO_ERROR_CONNDEAD == rc) {  // EOF occurs.
            d_isInvalidFlag = 1;
            retValue = ERROR_EOF;
            break;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_TIMEDOUT == rc) {
            *augStatus = ERR_TIMEOUT;
            break;
        }
        else if (rc < 0) { // Errors other than "AE", "EOF"/"TIMEDOUT" occur.
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

int btesos_TcpTimedChannel::getLocalAddress(bteso_IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->localAddress(result);
}

int btesos_TcpTimedChannel::getOption(int *result, int level, int option)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->socketOption(result, level, option);
}

int btesos_TcpTimedChannel::getPeerAddress(bteso_IPv4Address *result)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_socket_p);

    return d_socket_p->peerAddress(result);
}

int btesos_TcpTimedChannel::setOption(int level, int option, int value)
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
