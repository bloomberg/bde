// btlsos_tcpcbchannel.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcpcbchannel.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcpcbchannel_cpp,"$Id$ $CSID$")

#include <btlso_timereventmanager.h>
#include <btlso_streamsocket.h>
#include <btlsc_flag.h>
#include <btls_iovecutil.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslalg_scalardestructionprimitives.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_cstdio.h>
#include <bsl_functional.h>
#include <bsl_iterator.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                     // ===============================
                     // Local typedefs and enumerations
                     // ===============================

typedef btlsc::CbChannel::ReadCallback         ReadCb;
typedef btlsc::CbChannel::BufferedReadCallback BReadCb;
typedef btlsc::CbChannel::WriteCallback        WriteCb;

enum {
    k_RCALLBACK_SIZE  = sizeof(btlsc::CbChannel::ReadCallback),
    k_BRCALLBACK_SIZE = sizeof(btlsc::CbChannel::BufferedReadCallback),
    k_WCALLBACK_SIZE  = sizeof(btlsc::CbChannel::WriteCallback),

    k_RARENA_SIZE     = k_RCALLBACK_SIZE < k_BRCALLBACK_SIZE
                      ? k_BRCALLBACK_SIZE : k_RCALLBACK_SIZE,

    k_ARENA_SIZE      = k_RARENA_SIZE < k_WCALLBACK_SIZE
                      ? k_WCALLBACK_SIZE : k_RARENA_SIZE
};

enum {
    e_DEQUEUED  = -1,
    e_CANCELLED = -1,
    e_CONNECTION_CLOSED       = -1,
    e_INTERRUPT =  1
};

namespace btlsos {

                         // =======================
                         // class TcpCbChannel_RReg
                         // =======================

class TcpCbChannel_RReg {
public:
    enum {
        e_VFUNC2  = 0,
        e_VFUNC3  = 1
    };

    enum OperationCategory {
        e_BUFFERED,      // the request is for a buffered (non-vectored)
                         // operation
        e_NON_BUFFERED,  // the request is for a non-buffered (non-vectored)
                         // operation
        e_VECTORED_I     // the request is for a vectored operation
    };

    union {
        char                                d_arena[k_ARENA_SIZE];
        bsls::AlignmentUtil::MaxAlignedType d_align;  // for alignment
    } d_cb;

    // Data for the I/O operation
    union {
        struct {
            char              *d_buffer;  // the buffer for the next operation
            int                d_length;  // the length of the request for an
                                          // operation
        } d_s;
        struct {
            const btls::Iovec *d_buffers;
            int                      d_numBuffers;
        } d_vi;
    } d_data;
    int               d_requestLength;    // Original length requested.

    OperationCategory d_category;         // Operation category

    int               d_numSysCalls;

    int               d_flags;            // flags supplied in request.
    char              d_callbackType;

    // CREATORS
    TcpCbChannel_RReg(char          *buffer,
                      int            length,
                      int            numSysCalls,
                      const ReadCb&  callback,
                      int            flags);

    TcpCbChannel_RReg(const btls::Iovec *buffers,
                      int                numBuffers,
                      int                numSysCalls,
                      const ReadCb&      callback,
                      int                flags);

    TcpCbChannel_RReg(int            length,
                      int            numSysCalls,
                      const BReadCb& callback,
                      int            flags);

    ~TcpCbChannel_RReg();
        // Destroy this request.

    // MANIPULATORS
    void invoke(const char *buffer, int status, int augStatus) const;
        // Invoke the callback contained in this request passing it the
        // specified 'buffer', the specified 'status', and the specified
        // 'augStatus'.  The behavior is undefined unless the callback type (
        // as reported by 'd_callbackType') is 'VFUNC3'.

    void invoke(int status, int augStatus) const;
        // Invoke the callback contained in this request passing it the
        // specified 'status', and the specified 'augStatus'.  The behavior is
        // undefined unless the callback type (as reported by 'd_callbackType')
        // is 'VFUNC2'.

    void invokeConditionally(int status, int augStatus) const;
        // Invoke the callback contained in this requests passing it the
        // specified 'status' and the specified 'augStatus'.  If the contained
        // callback is VFUNC3, NULL address is passed for the first argument
        // (i.e., buffer).

};

// CREATORS

inline
TcpCbChannel_RReg::TcpCbChannel_RReg(
        int            length,
        int            numSysCalls,
        const BReadCb& callback,
        int            flags)
: d_requestLength(length)
, d_category(e_BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(e_VFUNC3) // Buffered
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);

    d_data.d_s.d_buffer = NULL;
    d_data.d_s.d_length = length;
    new (d_cb.d_arena) BReadCb(callback);
}

inline
TcpCbChannel_RReg::TcpCbChannel_RReg(const btls::Iovec *buffers,
                                     int                numBuffers,
                                     int                numSysCalls,
                                     const ReadCb&      callback,
                                     int                flags)
: d_requestLength(btls::IovecUtil::length(buffers, numBuffers))
, d_category(e_VECTORED_I)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(e_VFUNC2)
{
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffers);

    d_data.d_vi.d_buffers = buffers;
    d_data.d_vi.d_numBuffers = numBuffers;
    new (d_cb.d_arena) ReadCb(callback);
}

inline
TcpCbChannel_RReg::TcpCbChannel_RReg(
        char          *buffer,
        int            length,
        int            numSysCalls,
        const ReadCb&  callback,
        int            flags)
: d_requestLength(length)
, d_category(e_NON_BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(e_VFUNC2) // Buffered
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffer);

    d_data.d_s.d_buffer = buffer;
    d_data.d_s.d_length = length;
    new (d_cb.d_arena) ReadCb(callback);
}

inline
TcpCbChannel_RReg::~TcpCbChannel_RReg() {
    if (d_callbackType == e_VFUNC3) {
        bsl::function<void(const char *, int, int)> *cb =
                    (bsl::function<void(const char *, int, int)> *)
                        (void *)d_cb.d_arena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        BSLS_ASSERT(d_callbackType == e_VFUNC2);
        bsl::function<void(int, int)> *cb =
            (bsl::function<void(int, int)> *) (void *) d_cb.d_arena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
}

// MANIPULATORS
inline
void TcpCbChannel_RReg::invoke(int status, int augStatus) const
{
    BSLS_ASSERT(e_VFUNC2 == d_callbackType);
    bsl::function<void(int, int)> *cb =
        (bsl::function<void(int, int)> *)
        (void *) const_cast<char *>(d_cb.d_arena);
    (*cb)(status, augStatus);
}

inline
void TcpCbChannel_RReg::invoke(const char *buffer,
                               int         status,
                               int         augStatus) const
{
    BSLS_ASSERT(e_VFUNC3 == d_callbackType);
    BReadCb *cb = (BReadCb *)(void *) const_cast<char *>(d_cb.d_arena);
    (*cb)(buffer, status, augStatus);
}

inline
void TcpCbChannel_RReg::invokeConditionally(int status, int augStatus) const
{
    if (e_VFUNC2 == d_callbackType) {
        invoke(status, augStatus);
    }
    else {
        invoke(NULL, status, augStatus);
    }
}

bsl::ostream& operator<<(bsl::ostream& out, const TcpCbChannel_RReg& reg)
{
    switch(reg.d_category) {
      case TcpCbChannel_RReg::e_BUFFERED: {
        out << "B" << reg.d_requestLength << ", "
            << reg.d_data.d_s.d_length << ';' << bsl::flush;
      } break;
      case TcpCbChannel_RReg::e_NON_BUFFERED: {
        out << "N" << reg.d_requestLength << ", "
            << reg.d_data.d_s.d_length << ';' << bsl::flush;
      } break;
      case TcpCbChannel_RReg::e_VECTORED_I: {
         out << "V" << reg.d_requestLength << ", "
             << reg.d_data.d_vi.d_numBuffers << ';' << bsl::flush;
      } break;
    }
    return out;
}

                         // =======================
                         // class TcpCbChannel_WReg
                         // =======================

class TcpCbChannel_WReg {
public:
    enum OperationCategory {
        e_BUFFERED,      // the request is for a buffered (non-vectored)
                         // operation
        e_NON_BUFFERED,  // the request is for a non-buffered (non-vectored)
                         // operation
        e_VECTORED_I,    // the request is for a vectored read or write
                         // operation
        e_VECTORED_O     // the request is for a vectored write operation
    };

    bsl::function<void(int, int)> d_callback;

    // Data for the I/O operation
    union {
        struct {
            const char        *d_buffer;  // the buffer for the next operation
            int                d_length;  // the length of the request for an
                                          // operation
        } d_s;
        struct {
            const btls::Iovec *d_buffers;
            int                      d_numBuffers;
        } d_vi;
        struct {
            const btls::Ovec  *d_buffers;
            int                      d_numBuffers;
        } d_vo;
    } d_data;
    int               d_requestLength;    // Original length requested.
    OperationCategory d_category;         // Operation category

    int               d_numSysCalls;

    int               d_flags;       // flags supplied in request.

    // CREATORS
    TcpCbChannel_WReg(const char     *buffer,
                      int             length,
                      int             numSysCalls,
                      const WriteCb&  callback,
                      int             flags);

    TcpCbChannel_WReg(const btls::Iovec *buffers,
                      int                numBuffers,
                      int                numSysCalls,
                      const WriteCb&     callback,
                      int                flags);

    TcpCbChannel_WReg(const btls::Ovec *buffers,
                      int               numBuffers,
                      int               numSysCalls,
                      const WriteCb&    callback,
                      int               flags);

    TcpCbChannel_WReg(int            length,
                      int            numSysCalls,
                      const WriteCb& callback,
                      int            flags);

    ~TcpCbChannel_WReg();
        // Destroy this request.

    // MANIPULATORS
    void invoke(int, int) const;
    void invokeConditionally(int, int) const;
        // Invoke the callback contained in this request passing it the
        // specified 'status', and the specified 'augStatus'.
};

// CREATORS

inline
TcpCbChannel_WReg::TcpCbChannel_WReg(int            length,
                                     int            numSysCalls,
                                     const WriteCb& callback,
                                     int            flags)
: d_callback(callback)
, d_requestLength(length)
, d_category(e_BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);

    d_data.d_s.d_buffer = NULL;
    d_data.d_s.d_length = length;
}

inline
TcpCbChannel_WReg::TcpCbChannel_WReg(const btls::Iovec *buffers,
                                     int                numBuffers,
                                     int                numSysCalls,
                                     const WriteCb&     callback,
                                     int                flags)
: d_callback(callback)
, d_requestLength(btls::IovecUtil::length(buffers, numBuffers))
, d_category(e_VECTORED_I)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
{
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffers);

    d_data.d_vi.d_buffers = buffers;
    d_data.d_vi.d_numBuffers = numBuffers;
}

inline
TcpCbChannel_WReg::TcpCbChannel_WReg(const btls::Ovec *buffers,
                                     int               numBuffers,
                                     int               numSysCalls,
                                     const WriteCb&    callback,
                                     int               flags)
: d_callback(callback)
, d_requestLength(btls::IovecUtil::length(buffers, numBuffers))
, d_category(e_VECTORED_O)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
{
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffers);

    d_data.d_vo.d_buffers = buffers;
    d_data.d_vo.d_numBuffers = numBuffers;
}

inline
TcpCbChannel_WReg::TcpCbChannel_WReg(const char    *buffer,
                                     int            length,
                                     int            numSysCalls,
                                     const WriteCb& callback,
                                     int            flags)
: d_callback(callback)
, d_requestLength(length)
, d_category(e_NON_BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffer);

    d_data.d_s.d_buffer = buffer;
    d_data.d_s.d_length = length;
}

inline
TcpCbChannel_WReg::~TcpCbChannel_WReg() {
    // The callback will be destroyed on its own
}

// MANIPULATORS

inline
void TcpCbChannel_WReg::invoke(int status, int augStatus) const
{
    d_callback(status, augStatus);
}

inline
void TcpCbChannel_WReg::invokeConditionally(int status, int augStatus) const
{
    d_callback(status, augStatus);
}

}  // close package namespace

                     // ================================
                     // local function completeOperation
                     // ================================

static inline
int completeOperation(btlsos::TcpCbChannel_RReg *request,
                      bsl::vector<char>         *bufferPtr,
                      int                       *offset,
                      int                       *SP)
    // Try to satisfy the specified 'request' given that there is data in the
    // specified 'buffer' of the specified '*offset' length.  Return 0 if the
    // 'request' cannot be satisfied, and a non-zero value otherwise.  If the
    // 'request' is satisfied, its callback is invoked.  Otherwise, the
    // callback is not invoked.  The data is distributed as needed and 'offset'
    // is adjusted according to the number of bytes consumed.  The behavior is
    // undefined unless 'request', 'buffer', and 'offset' are valid.
{
    BSLS_ASSERT(request);
    BSLS_ASSERT(bufferPtr);
    BSLS_ASSERT(offset);

    if (0 == bufferPtr->size() || 0 == *offset) {
        return 0; // No data in buffer.                               // RETURN
    }

    char *buffer = &bufferPtr->front();
    BSLS_ASSERT(buffer && 0 < *offset);
    int rv = 0;

    switch(request->d_category) {
      case btlsos::TcpCbChannel_RReg::e_BUFFERED: {
          if (request->d_requestLength <= *offset - *SP) {
              int savedSP = *SP;
              *SP += request->d_requestLength;
              request->invoke(buffer + savedSP, request->d_requestLength, 0);
              if (0 == savedSP) {
                  *offset -= *SP;
                  bsl::memcpy(buffer, buffer + *SP, *offset);
                  *SP = 0;
              }
              rv = 1;
          }
          else if (request->d_numSysCalls == 1 && 0 < *offset) {
              // Not enough data in the buffer and the call is 'raw'.
              request->invoke(buffer, *offset, 0);
              *offset = 0;
              rv = 1;
          }
      } break;
      case btlsos::TcpCbChannel_RReg::e_NON_BUFFERED: {
          BSLS_ASSERT(request->d_requestLength ==
                                                 request->d_data.d_s.d_length);
          int dataLength = *offset - *SP;

          if (request->d_requestLength <= dataLength) {
              int savedSP = *SP;
              *SP += request->d_requestLength;
              bsl::memcpy(request->d_data.d_s.d_buffer,
                          buffer + savedSP,
                          request->d_requestLength);

              request->invoke(request->d_requestLength, 0);
              if (0 == savedSP) {
                  *offset -= *SP;
                  bsl::memcpy(buffer, buffer + *SP, *offset);
                  *SP = 0;
              }
              rv = 1;
          }
          else {
              bsl::memcpy(request->d_data.d_s.d_buffer,
                          buffer + *SP,
                          dataLength);
              request->d_data.d_s.d_length = request->d_requestLength -
                  dataLength;
              request->d_data.d_s.d_buffer += dataLength;
              *offset = *SP;
          }
      } break;
      case btlsos::TcpCbChannel_RReg::e_VECTORED_I: {
          int s = btls::IovecUtil::scatter(request->d_data.d_vi.d_buffers,
                                          request->d_data.d_vi.d_numBuffers,
                                          buffer + *SP, *offset - *SP);
          int savedSP = *SP;
          *SP += s;
          request->invoke(s, 0);
          if (savedSP == 0) {
              *offset -= *SP;
              bsl::memcpy(buffer, buffer + *SP, *offset);
              *SP = 0;
          }
          rv = 1;
      } break;
      default: {
          BSLS_ASSERT("Invalid operation category for the read request."
                         && 0);
      }
    }
    return rv;
}

                       // ===========================
                       // local function dequeue<REG>
                       // ===========================

template <class REG>
inline void
dequeue(bsl::deque<REG *> *queue, int status, int augStatus, bdlma::Pool *pool)
    // Invoke each callback contained in the specified 'queue' with the
    // specified 'status' and the specified 'augStatus' in the reverse order,
    // remove each request from the queue and deallocate memory associated with
    // the request using the specified 'pool'.  The behavior is undefined if
    // either 'pool' or 'queue' is 0.
{
    BSLS_ASSERT(queue);
    int numElements = static_cast<int>(queue->size());

    for (int i = 0; i < numElements; ++i) {
        REG *reg = (*queue)[numElements - 1 - i];
        BSLS_ASSERT(reg);

        reg->invokeConditionally(status, augStatus);
        pool->deleteObjectRaw(reg);
    }
    queue->clear();
}

                     // ===============================
                     // local function initializeBuffer
                     // ===============================

static inline
void initializeBuffer(bsl::vector<char>                       *buffer,
                      btlso::StreamSocket<btlso::IPv4Address> *socket_p,
                      int                                      option)
{
    enum { k_DEFAULT_BUFFER_SIZE = 8192 };
    int result;
    int s = socket_p->socketOption(&result,
                                   btlso::SocketOptUtil::k_SOCKETLEVEL,
                                   option);
    if (!s) {
        BSLS_ASSERT(0 < result);
        buffer->resize(result);
    }
    else {
        buffer->resize(k_DEFAULT_BUFFER_SIZE);
    }
}

namespace btlsos {

// ============================================================================
//                           END LOCAL DEFINITIONS
// ============================================================================

                            // ------------------
                            // class TcpCbChannel
                            // ------------------

// PRIVATE MANIPULATORS

void TcpCbChannel::initializeReadBuffer(int size)
{
    if (size > 0) {
        d_readBuffer.resize(size);
    }
    else {
        initializeBuffer(&d_readBuffer,
                         d_socket_p,
                         btlso::SocketOptUtil::k_RECEIVEBUFFER);
    }
}

void TcpCbChannel::initializeWriteBuffer(int size)
{
    if (size > 0) {
        d_writeBuffer.resize(size);
    }
    else {
        initializeBuffer(&d_writeBuffer,
                         d_socket_p,
                         btlso::SocketOptUtil::k_SENDBUFFER);
    }
}

void TcpCbChannel::bufferedReadCb()
{
    BSLS_ASSERT(d_socket_p);
    BSLS_ASSERT(d_readRequests.size());
    BSLS_ASSERT(0 == d_currentReadRequest_p);
    BSLS_ASSERT(0 == d_readBufferSP);
    d_currentReadRequest_p = d_readRequests.back();
    BSLS_ASSERT(d_currentReadRequest_p);
    BSLS_ASSERT(TcpCbChannel_RReg::e_BUFFERED ==
           d_currentReadRequest_p->d_category);
    BSLS_ASSERT(TcpCbChannel_RReg::e_VFUNC3 ==
           d_currentReadRequest_p->d_callbackType);

    if (0 == d_readBuffer.size()) {
        initializeReadBuffer();
    }

    int requestLength = d_currentReadRequest_p->d_requestLength;
    int numBytesRemaining = requestLength - d_readBufferOffset;
    int available = static_cast<int>(d_readBuffer.size()) - d_readBufferOffset;
    BSLS_ASSERT(available >= numBytesRemaining);

    int s = d_socket_p->read(&d_readBuffer.front() + d_readBufferOffset,
                             available); // Fill in the read buffer.

    if (0 < s) {
        // Read 's' bytes from the socket.
        --d_currentReadRequest_p->d_numSysCalls;
        d_readBufferOffset += s;
        // We have accumulated 'd_readBufferOffset' bytes for the request
        if (d_readBufferOffset >= requestLength) {
            // Operation is completed
            d_readBufferSP = requestLength;
            d_currentReadRequest_p->invoke(&d_readBuffer.front(),
                                           requestLength,
                                           0);
            d_readBufferOffset -= d_readBufferSP;
            if (d_readBufferOffset) {
                bsl::memcpy(&d_readBuffer.front(),
                            &d_readBuffer.front() + d_readBufferSP,
                            d_readBufferOffset);
            }
            d_readBufferSP = 0;
        }
        else if (0 == d_currentReadRequest_p->d_numSysCalls) {
            // Operation is completed
            d_currentReadRequest_p->invoke(&d_readBuffer.front(),
                                           d_readBufferOffset, 0);
            d_readBufferOffset = 0;
        }
        else {
            // Operation is incomplete, and partial result is authorized
            // => do nothing, just return.
            d_currentReadRequest_p = NULL;
            return;                                                   // RETURN
        }
    }
    else if (btlso::SocketHandle::e_ERROR_EOF == s) {
        // Connection was closed
        invalidateRead();
        d_currentReadRequest_p->invoke(NULL, e_CONNECTION_CLOSED, 0);
    } else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
        if (d_currentReadRequest_p->d_flags
                                        & btlsc::Flag::k_ASYNC_INTERRUPT) {
            bsl::deque<TcpCbChannel_RReg *>
                                 toBeDispatched(d_readRequests, d_allocator_p);
            toBeDispatched.pop_back();
            d_readRequests.erase(d_readRequests.begin(),
                                 d_readRequests.begin() +
                                                    d_readRequests.size() - 1);
            d_currentReadRequest_p->invoke(&d_readBuffer.front(),
                                           d_readBufferOffset, e_INTERRUPT);

            dequeue(&toBeDispatched, 0, e_DEQUEUED, &d_rrequestPool);
        }
        else {
            // Restart on the interrupt.
            d_currentReadRequest_p = NULL;
            return;                                                   // RETURN
        }
    }
    else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
        d_currentReadRequest_p = NULL;

        // Fake wake up from the event manager.  The number of system calls is
        // not counted.

        return;                                                       // RETURN
    }
    else {
        BSLS_ASSERT(s < 0);
        BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
        BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);
        invalidateRead(); // Hard error on channel.
        d_currentReadRequest_p->invoke(NULL, s - 1, 0);
        d_readRequests.pop_back();

        dequeue(&d_readRequests, 0, e_DEQUEUED, &d_rrequestPool);
        d_readRequests.push_back(d_currentReadRequest_p);
    }

    // Destroy current request
    d_readRequests.pop_back();
    d_rrequestPool.deleteObjectRaw(d_currentReadRequest_p);

    while (d_readRequests.size()) {
        d_currentReadRequest_p = d_readRequests.back();
        BSLS_ASSERT(d_currentReadRequest_p);
        if (!completeOperation(d_currentReadRequest_p, &d_readBuffer,
                               &d_readBufferOffset, &d_readBufferSP)) {
            break;
        }

        d_readRequests.pop_back();
        d_rrequestPool.deleteObjectRaw(d_currentReadRequest_p);
        d_currentReadRequest_p = NULL;
    }
    if (d_readRequests.size()) {
        while (TcpCbChannel_RReg::e_BUFFERED !=
                                       d_currentReadRequest_p->d_category) {
            if (0 !=
                 d_rManager_p->registerSocketEvent(
                                                  d_socket_p->handle(),
                                                  btlso::EventType::e_READ,
                                                  d_readFunctor)) {
                d_currentReadRequest_p->invoke(0, e_CANCELLED);
                d_readRequests.pop_back();
                d_rrequestPool.deleteObjectRaw(d_currentReadRequest_p);
            }
            else {
                break;
            }
            if (d_readRequests.size()) {
                d_currentReadRequest_p = d_readRequests.back();
            }
            else {
                d_rManager_p->deregisterSocketEvent(
                                                 d_socket_p->handle(),
                                                 btlso::EventType::e_READ);
                break;
            }
        }
    }
    else {
        d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                           btlso::EventType::e_READ);
    }

    // Invariant must be maintained.
    d_currentReadRequest_p = NULL;
}

void TcpCbChannel::readCb()
{
    BSLS_ASSERT(d_readRequests.size());
    BSLS_ASSERT(d_currentReadRequest_p == NULL);
    d_currentReadRequest_p = d_readRequests.back();
    BSLS_ASSERT(d_currentReadRequest_p);

    switch(d_currentReadRequest_p->d_category) {
      case TcpCbChannel_RReg::e_NON_BUFFERED:
      {
          char *buffer = d_currentReadRequest_p->d_data.d_s.d_buffer;
          int numBytes = d_currentReadRequest_p->d_data.d_s.d_length;
          int requestLength = d_currentReadRequest_p->d_requestLength;
          BSLS_ASSERT(buffer);
          BSLS_ASSERT(0 <= numBytes);

          int s = d_socket_p->read(buffer, numBytes);

          if (0 < s) {
              --d_currentReadRequest_p->d_numSysCalls;
              d_currentReadRequest_p->d_data.d_s.d_buffer += s;
              numBytes -= s;
              d_currentReadRequest_p->d_data.d_s.d_length = numBytes;
              BSLS_ASSERT(0 <= d_currentReadRequest_p->d_data.d_s.d_length);

              if (numBytes == 0 || d_currentReadRequest_p->d_numSysCalls == 0)
              {
                  // Operation is completed
                  d_currentReadRequest_p->invoke(requestLength - numBytes, 0);
              }
              else {
                  // Operation is incomplete => do nothing.
                  d_currentReadRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (btlso::SocketHandle::e_ERROR_EOF == s) {
              // Connection was closed
              invalidateRead();
              d_currentReadRequest_p->invoke(e_CONNECTION_CLOSED, 0);
              break;
          } else if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
              if (d_currentReadRequest_p->d_flags &
                  btlsc::Flag::k_ASYNC_INTERRUPT)
              {
                  d_currentReadRequest_p->invoke(requestLength - numBytes,
                                                 e_INTERRUPT);
              }
              else {
                  d_currentReadRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (s == btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentReadRequest_p = NULL;
              return;                                                 // RETURN
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);
              d_currentReadRequest_p->invoke(s - 1, 0);
          }
      } break;
      case TcpCbChannel_RReg::e_VECTORED_I: {
          const btls::Iovec *buffers =
              d_currentReadRequest_p->d_data.d_vi.d_buffers;
          BSLS_ASSERT(buffers);

          int numBuffers = d_currentReadRequest_p->d_data.d_vi.d_numBuffers;
          int requestLength = d_currentReadRequest_p->d_requestLength;
          int numBytes = btls::IovecUtil::length(buffers, numBuffers);

          int s = d_socket_p->readv(buffers, numBuffers);

          if (0 < s) {
              --d_currentReadRequest_p->d_numSysCalls;
              numBytes -= s;
              BSLS_ASSERT(0 <= numBytes);
              // Note: we support only RAW VECTORIZED operations for
              // performance reasons.  If a vectorized request is not satisfied
              // completely in one read, the user of the channel can do
              // adjustments more efficiently.

              BSLS_ASSERT(0 == d_currentReadRequest_p->d_numSysCalls);
              d_currentReadRequest_p->invoke(requestLength - numBytes, 0);
          }
          else if (btlso::SocketHandle::e_ERROR_EOF == s) {
              // Connection was closed
              invalidateRead();
              d_currentReadRequest_p->invoke(e_CONNECTION_CLOSED, 0);
              break;
          } else if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
              if (d_currentReadRequest_p->d_flags &
                  btlsc::Flag::k_ASYNC_INTERRUPT)
              {
                  d_currentReadRequest_p->invoke(requestLength - numBytes,
                                                 e_INTERRUPT);
              }
              else {
                  d_currentReadRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (s == btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentReadRequest_p = NULL;
              return;                                                 // RETURN
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);

              d_currentReadRequest_p->invoke(s - 1, 0);
          }
      } break;
      default: {
          BSLS_ASSERT("Invalid operation category for the read request."
                         && 0);
      }
    }

    // Destroy current request
    d_readRequests.pop_back();
    d_rrequestPool.deleteObjectRaw(d_currentReadRequest_p);
    d_currentReadRequest_p = NULL;

    if (d_readRequests.size()) {
        d_currentReadRequest_p = d_readRequests.back();
        BSLS_ASSERT(d_currentReadRequest_p);

        while (TcpCbChannel_RReg::e_BUFFERED ==
                                      d_currentReadRequest_p->d_category)
        {
            if (0 != d_rManager_p->registerSocketEvent(
                                                  d_socket_p->handle(),
                                                  btlso::EventType::e_READ,
                                                  d_bufferedReadFunctor))
            {
                d_currentReadRequest_p->invoke(NULL, 0, e_CANCELLED);
                d_readRequests.pop_back();
                d_rrequestPool.deleteObjectRaw(d_currentReadRequest_p);
            }
            else
            {
                break;
            }
            if (d_readRequests.size()) {
                d_currentReadRequest_p = d_readRequests.back();
            }
            else {
                d_rManager_p->deregisterSocketEvent(
                                                 d_socket_p->handle(),
                                                 btlso::EventType::e_READ);
                break;
            }
        }
    }
    else {
        d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                            btlso::EventType::e_READ);
    }

    // Invariant must be maintained.
    d_currentReadRequest_p = NULL;
}

void TcpCbChannel::bufferedWriteCb() {
    BSLS_ASSERT(d_socket_p);
    BSLS_ASSERT(d_writeRequests.size());
    BSLS_ASSERT(TcpCbChannel_WReg::e_BUFFERED ==
                                           d_writeRequests.back()->d_category);

    int numBytesToWrite = 0;
    int numPendingRequests = static_cast<int>(d_writeRequests.size());
    for (int i = numPendingRequests - 1; 0 <= i; --i) {
        if (d_writeRequests[i]->d_category
                                       != TcpCbChannel_WReg::e_BUFFERED) {
            break;
        }
        numBytesToWrite += d_writeRequests[i]->d_data.d_s.d_length;
    }

    BSLS_ASSERT(numBytesToWrite <= d_writeBufferOffset);
    BSLS_ASSERT(0 < numBytesToWrite);

    int s = d_socket_p->write(&d_writeBuffer.front(), numBytesToWrite);

    if (0 < s) {
        // Wrote 's' bytes into the socket
        int numBytesConsumed = 0;
        while (d_writeRequests.size()) {
            d_currentWriteRequest_p = d_writeRequests.back();
            BSLS_ASSERT(d_currentWriteRequest_p);
            BSLS_ASSERT(d_currentWriteRequest_p->d_data.d_s.d_length);

            if (d_currentWriteRequest_p->d_data.d_s.d_length <= s) {
                // Request completed successfully.
                s -= d_currentWriteRequest_p->d_data.d_s.d_length;
                d_currentWriteRequest_p->invoke(
                        d_currentWriteRequest_p->d_requestLength, 0);
                numBytesConsumed +=
                    d_currentWriteRequest_p->d_data.d_s.d_length;
                d_writeRequests.pop_back();
                d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
                d_currentWriteRequest_p = NULL;
            }
            else {
                break;
            }
        }
        if (d_currentWriteRequest_p) {
            d_currentWriteRequest_p->d_data.d_s.d_length -= s;
            numBytesConsumed += s;
            d_writeBufferOffset -= numBytesConsumed;
            bsl::memcpy(&d_writeBuffer.front(),
                        &d_writeBuffer.front() + numBytesConsumed,
                        d_writeBufferOffset);

            while (TcpCbChannel_WReg::e_BUFFERED !=
                                         d_currentWriteRequest_p->d_category)
            {
                if (0 != d_wManager_p->registerSocketEvent(
                                                 d_socket_p->handle(),
                                                 btlso::EventType::e_WRITE,
                                                 d_writeFunctor))
                {
                    d_currentWriteRequest_p->invoke(0, e_CANCELLED);
                    d_writeRequests.pop_back();
                    d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
                }
                else
                {
                    break;
                }
                if (d_writeRequests.size()) {
                    d_currentWriteRequest_p = d_writeRequests.back();
                }
                else {
                    d_wManager_p->deregisterSocketEvent(
                                                d_socket_p->handle(),
                                                btlso::EventType::e_WRITE);
                    break;
                }
            }
        }
        else {
            BSLS_ASSERT(0 == s);
            BSLS_ASSERT(numBytesConsumed == d_writeBufferOffset);
            d_writeBufferOffset = 0;
            BSLS_ASSERT(0 == d_writeRequests.size());
            d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                               btlso::EventType::e_WRITE);
        }
        d_currentWriteRequest_p = NULL;
        return;                                                       // RETURN
    }
    else if (btlso::SocketHandle::e_ERROR_EOF == s) {
        // Connection was closed
        invalidateWrite();
        d_currentWriteRequest_p->invoke(e_CONNECTION_CLOSED, 0);

    } else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
        if (d_currentWriteRequest_p->d_flags
                                         & btlsc::Flag::k_ASYNC_INTERRUPT) {
            bsl::deque<TcpCbChannel_WReg *>
                                toBeDispatched(d_writeRequests, d_allocator_p);
            toBeDispatched.pop_back();
            d_writeRequests.erase(d_writeRequests.begin(),
                                  d_writeRequests.begin() +
                                                   d_writeRequests.size() - 1);
            d_currentWriteRequest_p->invoke(
                    d_currentWriteRequest_p->d_requestLength -
                    d_currentWriteRequest_p->d_data.d_s.d_length,
                    e_INTERRUPT);

            dequeue(&toBeDispatched, 0, e_DEQUEUED, &d_wrequestPool);
        }
        else {
            // Restart on the interrupt.
            d_currentWriteRequest_p = NULL;
            return;                                                   // RETURN
        }
    }
    else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
        d_currentWriteRequest_p = NULL;

        // Fake wake up from the event manager.  The number of system calls is
        // not counted.

        return;                                                       // RETURN
    }
    else {
        // Hard error on the channel -> invalidate and dequeue.
        BSLS_ASSERT(s < 0);
        BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
        BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);
        invalidateWrite();
        d_writeRequests.pop_back();
        d_currentWriteRequest_p->invoke(s - 1, 0);
        dequeue(&d_writeRequests, 0, e_DEQUEUED, &d_wrequestPool);
        d_writeRequests.push_back(d_currentWriteRequest_p);
    }

    d_writeRequests.pop_back();
    d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
    d_currentWriteRequest_p = NULL;

    if (d_writeRequests.size()) {
        while (TcpCbChannel_WReg::e_BUFFERED !=
                                      d_currentWriteRequest_p->d_category)
        {
            if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                             btlso::EventType::e_WRITE,
                                             d_writeFunctor))
            {
                d_currentWriteRequest_p->invoke(0, e_CANCELLED);
                d_writeRequests.pop_back();
                d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
            }
            else
            {
                break;
            }
            if (d_writeRequests.size()) {
                d_currentWriteRequest_p = d_writeRequests.back();
            }
            else {
                d_wManager_p->deregisterSocketEvent(
                                                d_socket_p->handle(),
                                                btlso::EventType::e_WRITE);
                break;
            }
        }
    }
    else {
        d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                           btlso::EventType::e_WRITE);
    }

    // Invariant must be maintained.
    d_currentWriteRequest_p = NULL;
}

void TcpCbChannel::writeCb() {
    // There is space in the socket's system buffer for data.
    BSLS_ASSERT(d_writeRequests.size());
    BSLS_ASSERT(NULL == d_currentWriteRequest_p);
    d_currentWriteRequest_p = d_writeRequests.back();
    BSLS_ASSERT(d_currentWriteRequest_p);

    switch(d_currentWriteRequest_p->d_category) {
      case TcpCbChannel_WReg::e_NON_BUFFERED: {
          const char *buffer = d_currentWriteRequest_p->d_data.d_s.d_buffer;
          int numBytes = d_currentWriteRequest_p->d_data.d_s.d_length;
          int requestLength = d_currentWriteRequest_p->d_requestLength;

          int s = d_socket_p->write(buffer, numBytes);
          if (0 < s) {
              // Wrote 's' bytes into the socket.
              --d_currentWriteRequest_p->d_numSysCalls;
              d_currentWriteRequest_p->d_data.d_s.d_buffer += s;
              numBytes -= s;
              d_currentWriteRequest_p->d_data.d_s.d_length = numBytes;
              if (numBytes == 0 ||
                  d_currentWriteRequest_p->d_numSysCalls == 0)
              {
                  d_currentWriteRequest_p->invoke(requestLength - numBytes, 0);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return; // Do nothing - operation is incomplete.    // RETURN
              }
          }
          else if (btlso::SocketHandle::e_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(e_CONNECTION_CLOSED, 0);
              break;
          } else if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btlsc::Flag::k_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(requestLength - numBytes,
                                                  e_INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (s == btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;                                                 // RETURN
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);
              d_currentWriteRequest_p->invoke(s - 1, 0);
          }
      } break;

      case TcpCbChannel_WReg::e_VECTORED_I: {
          const btls::Iovec *buffers =
              d_currentWriteRequest_p->d_data.d_vi.d_buffers;
          int numBuffers = d_currentWriteRequest_p->d_data.d_vi.d_numBuffers;

          int s = d_socket_p->writev(buffers, numBuffers);
          if (0 < s) {
              // Wrote 's' bytes into the socket.
              --d_currentWriteRequest_p->d_numSysCalls;

              d_currentWriteRequest_p->invoke(s, 0);
          }
          else if (btlso::SocketHandle::e_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(e_CONNECTION_CLOSED, 0);
              break;
          } else if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btlsc::Flag::k_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(0,
                                                  e_INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (s == btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;                                                 // RETURN
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);

              d_currentWriteRequest_p->invoke(s - 1, 0);
          }
      } break;

      case TcpCbChannel_WReg::e_VECTORED_O: {
          const btls::Ovec *buffers =
              d_currentWriteRequest_p->d_data.d_vo.d_buffers;
          int numBuffers = d_currentWriteRequest_p->d_data.d_vo.d_numBuffers;

          int s = d_socket_p->writev(buffers, numBuffers);
          if (0 < s) {
              // Wrote 's' bytes into the socket.
              --d_currentWriteRequest_p->d_numSysCalls;
              d_currentWriteRequest_p->invoke(s, 0);
          }
          else if (btlso::SocketHandle::e_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(e_CONNECTION_CLOSED, 0);
              break;
          } else if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btlsc::Flag::k_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(0, e_INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;                                             // RETURN
              }
          }
          else if (s == btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;                                                 // RETURN
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != btlso::SocketHandle::e_ERROR_WOULDBLOCK);

              d_currentWriteRequest_p->invoke(s - 1, 0);
          }
      } break;
      default: {
          BSLS_ASSERT("Invalid operation category for a write request."
                         && 0);
      }
    }

    // Destroy current request
    d_writeRequests.pop_back();
    d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
    d_currentWriteRequest_p = NULL;

    if (d_writeRequests.size()) {
        d_currentWriteRequest_p = d_writeRequests.back();
        BSLS_ASSERT(d_currentWriteRequest_p);
        while ((int) TcpCbChannel_WReg::e_BUFFERED ==
               (int) d_currentWriteRequest_p->d_category)
        {   // will replace the current event.
            if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                              btlso::EventType::e_WRITE,
                                              d_bufferedWriteFunctor))
            {
                d_currentWriteRequest_p->invoke(0, e_CANCELLED);
                d_writeRequests.pop_back();
                d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
            }
            else
            {
                break;
            }
            if (d_writeRequests.size()) {
                d_currentWriteRequest_p = d_writeRequests.back();
            }
            else {
                d_wManager_p->deregisterSocketEvent(
                                                d_socket_p->handle(),
                                                btlso::EventType::e_WRITE);
                break;
            }
        }
    }
    else {
        d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                            btlso::EventType::e_WRITE);

    }

    d_currentWriteRequest_p = NULL;
    BSLS_ASSERT(NULL == d_currentWriteRequest_p);
}

// CREATORS

TcpCbChannel::TcpCbChannel(
                       btlso::StreamSocket<btlso::IPv4Address> *sSocket,
                       btlso::TimerEventManager                *manager,
                       bslma::Allocator                        *basicAllocator)
: d_socket_p(sSocket)
, d_rManager_p(manager)
, d_wManager_p(manager)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferSP(0)
, d_writeBuffer(basicAllocator)
, d_writeBufferOffset(0)
, d_rrequestPool(sizeof(TcpCbChannel_RReg), basicAllocator)
, d_wrequestPool(sizeof(TcpCbChannel_WReg), basicAllocator)
, d_allocator_p(basicAllocator)
, d_isInvalidReadFlag(0)
, d_isInvalidWriteFlag(0)
, d_currentReadRequest_p(NULL)
, d_currentWriteRequest_p(NULL)
{
    BSLS_ASSERT(d_rManager_p);
    BSLS_ASSERT(d_wManager_p);
    BSLS_ASSERT(sSocket);
    // Initialize functors.
    d_bufferedReadFunctor
        = bsl::function<void()>(
              bsl::allocator_arg_t(),
              bsl::allocator<bsl::function<void()> >(d_allocator_p),
              bdlf::MemFnUtil::memFn(&TcpCbChannel::bufferedReadCb, this));

    d_readFunctor
        = bsl::function<void()>(
               bsl::allocator_arg_t(),
               bsl::allocator<bsl::function<void()> >(d_allocator_p),
               bdlf::MemFnUtil::memFn(&TcpCbChannel::readCb, this));

    d_bufferedWriteFunctor
        = bsl::function<void()>(
             bsl::allocator_arg_t(),
             bsl::allocator<bsl::function<void()> >(d_allocator_p),
             bdlf::MemFnUtil::memFn(&TcpCbChannel::bufferedWriteCb, this));

    d_writeFunctor
        = bsl::function<void()>(
              bsl::allocator_arg_t(),
              bsl::allocator<bsl::function<void()> >(d_allocator_p),
              bdlf::MemFnUtil::memFn(&TcpCbChannel::writeCb, this));

    d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
}

TcpCbChannel::TcpCbChannel(
                       btlso::StreamSocket<btlso::IPv4Address> *sSocket,
                       btlso::TimerEventManager                *rManager,
                       btlso::TimerEventManager                *wManager,
                       bslma::Allocator                        *basicAllocator)
: d_socket_p(sSocket)
, d_rManager_p(rManager)
, d_wManager_p(wManager)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferSP(0)
, d_writeBuffer(basicAllocator)
, d_writeBufferOffset(0)
, d_rrequestPool(sizeof(TcpCbChannel_RReg), basicAllocator)
, d_wrequestPool(sizeof(TcpCbChannel_WReg), basicAllocator)
, d_allocator_p(basicAllocator)
, d_isInvalidReadFlag(0)
, d_isInvalidWriteFlag(0)
, d_currentReadRequest_p(NULL)
, d_currentWriteRequest_p(NULL)
{
    BSLS_ASSERT(d_rManager_p);
    BSLS_ASSERT(d_wManager_p);
    BSLS_ASSERT(sSocket);
    // Initialize functors.
    d_bufferedReadFunctor
        = bsl::function<void()>(
              bsl::allocator_arg_t(),
              bsl::allocator<bsl::function<void()> >(d_allocator_p),
              bdlf::MemFnUtil::memFn(&TcpCbChannel::bufferedReadCb, this));

    d_readFunctor
        = bsl::function<void()>(
               bsl::allocator_arg_t(),
               bsl::allocator<bsl::function<void()> >(d_allocator_p),
               bdlf::MemFnUtil::memFn(&TcpCbChannel::readCb, this));

    d_bufferedWriteFunctor
        = bsl::function<void()>(
             bsl::allocator_arg_t(),
             bsl::allocator<bsl::function<void()> >(d_allocator_p),
             bdlf::MemFnUtil::memFn(&TcpCbChannel::bufferedWriteCb, this));

    d_writeFunctor
        = bsl::function<void()>(
              bsl::allocator_arg_t(),
              bsl::allocator<bsl::function<void()> >(d_allocator_p),
              bdlf::MemFnUtil::memFn(&TcpCbChannel::writeCb, this));

    d_socket_p->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
}

TcpCbChannel::~TcpCbChannel() {
    invalidate();
    BSLS_ASSERT(d_currentWriteRequest_p == NULL);
    BSLS_ASSERT(d_currentReadRequest_p == NULL);
    cancelAll(); // Cancel all will deregister it.
}

// MANIPULATORS

///Read section
///------------

int TcpCbChannel::read(char                *buffer,
                       int                  numBytes,
                       const ReadCallback&  readCallback,
                       int                  flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;                                                    // RETURN
    }

    TcpCbChannel_RReg *request =
        new (d_rrequestPool) TcpCbChannel_RReg (buffer, numBytes,
                                                  numBytes,
                                                  readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;                                                 // RETURN
        }

        if (0 != d_rManager_p->registerSocketEvent(
                                                  d_socket_p->handle(),
                                                  btlso::EventType::e_READ,
                                                  d_readFunctor)) {

            request->invoke(0, e_CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }

    }

    d_readRequests.push_front(request);
    return 0;
}

int TcpCbChannel::readRaw(char                *buffer,
                          int                  numBytes,
                          const ReadCallback&  readCallback,
                          int                  flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;                                                    // RETURN
    }

    TcpCbChannel_RReg *request =
        new (d_rrequestPool) TcpCbChannel_RReg (buffer, numBytes,
                                                  1,
                                                  readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;                                                 // RETURN
        }

        if (0 != d_rManager_p->registerSocketEvent(
                                                  d_socket_p->handle(),
                                                  btlso::EventType::e_READ,
                                                  d_readFunctor)) {

            request->invoke(0, e_CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }

    d_readRequests.push_front(request);
    return 0;
}

int TcpCbChannel::readv(const btls::Iovec   *buffers,
                        int                  numBuffers,
                        const ReadCallback&  readCallback,
                        int                  flags)
{
    return readvRaw(buffers, numBuffers, readCallback, flags);
}

int
TcpCbChannel::readvRaw(const btls::Iovec   *buffers,
                       int                  numBuffers,
                       const ReadCallback&  readCallback,
                       int                  flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidReadFlag) {
        return -1;                                                    // RETURN
    }

    TcpCbChannel_RReg *request =
        new (d_rrequestPool) TcpCbChannel_RReg (buffers, numBuffers,
                                                  1, readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;                                                 // RETURN
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                         btlso::EventType::e_READ,
                                         d_readFunctor)){

            request->invoke(0, e_CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }

    }
    d_readRequests.push_front(request);
    return 0;
}

int
TcpCbChannel::bufferedRead(int                         numBytes,
                           const BufferedReadCallback& bufferedReadCallback,
                           int                         flags)
{
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;                                                    // RETURN
    }

    TcpCbChannel_RReg *request =
        new (d_rrequestPool) TcpCbChannel_RReg (numBytes, numBytes,
                                              bufferedReadCallback,
                                              flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;                                                 // RETURN
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                         btlso::EventType::e_READ,
                                         d_bufferedReadFunctor)){

            request->invoke(0, 0, e_CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }

    }

    d_readRequests.push_front(request);
    return 0;
}

int TcpCbChannel::bufferedReadRaw(
                              int                         numBytes,
                              const BufferedReadCallback& bufferedReadCallback,
                              int                         flags)
{
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;                                                    // RETURN
    }

    TcpCbChannel_RReg *request =
        new (d_rrequestPool) TcpCbChannel_RReg(numBytes, 1,
                                                      bufferedReadCallback,
                                                      flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request,
                              &d_readBuffer,
                              &d_readBufferOffset,
                              &d_readBufferSP))
        {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;                                                 // RETURN
        }

        if (0 != d_rManager_p->registerSocketEvent(
                                                  d_socket_p->handle(),
                                                  btlso::EventType::e_READ,
                                                  d_bufferedReadFunctor))
        {
            request->invoke(0, 0, e_CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }

    d_readRequests.push_front(request);
    return 0;
}

///Write section
///-------------

int TcpCbChannel::write(const char           *buffer,
                        int                   numBytes,
                        const WriteCallback&  writeCallback,
                        int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->write(buffer, numBytes);

        if (s == numBytes) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (0 < s) {
            // Partial result.
            TcpCbChannel_WReg *request =
                new (d_wrequestPool)
                    TcpCbChannel_WReg(buffer + s, numBytes,
                                                         numBytes,
                                                         writeCallback, flags);
            request->d_data.d_s.d_length -= s;
            d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             TcpCbChannel_WReg(buffer, numBytes,
                                                              numBytes,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             TcpCbChannel_WReg(buffer, numBytes,
                                                              numBytes,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                         btlso::EventType::e_WRITE,
                                         d_writeFunctor)){
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        TcpCbChannel_WReg *request =
            new (d_wrequestPool)
                     TcpCbChannel_WReg(buffer, numBytes,
                                                      numBytes,
                                                      writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int TcpCbChannel::writeRaw(const char           *buffer,
                           int                   numBytes,
                           const WriteCallback&  writeCallback,
                           int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->write(buffer, numBytes);
        if (0 < s) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             TcpCbChannel_WReg(buffer, numBytes,
                                                              1,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             TcpCbChannel_WReg(buffer, numBytes,
                                                              1,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                         btlso::EventType::e_WRITE,
                                         d_writeFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(buffer, numBytes,
                                                      1,
                                                      writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int TcpCbChannel::writev(const btls::Ovec     *buffers,
                         int                   numBuffers,
                         const WriteCallback&  writeCallback,
                         int                   flags)
{
    return writevRaw(buffers, numBuffers, writeCallback, flags);
}

int TcpCbChannel::writev(const btls::Iovec    *buffers,
                         int                   numBuffers,
                         const WriteCallback&  writeCallback,
                         int                   flags)
{
    return writevRaw(buffers, numBuffers, writeCallback, flags);
}

int TcpCbChannel::writevRaw(const btls::Ovec     *buffers,
                            int                   numBuffers,
                            const WriteCallback&  writeCallback,
                            int                   flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        if (0 < s) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          btlso::EventType::e_WRITE,
                                          d_writeFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(buffers, numBuffers,
                                                 1,
                                                 writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int TcpCbChannel::writevRaw(const btls::Iovec    *buffers,
                            int                   numBuffers,
                            const WriteCallback&  writeCallback,
                            int                   flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);

        if (0 < s) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          btlso::EventType::e_WRITE,
                                          d_writeFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(buffers, numBuffers,
                                                 1,
                                                 writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int TcpCbChannel::bufferedWrite(const char           *buffer,
                                int                   numBytes,
                                const WriteCallback&  writeCallback,
                                int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeBuffer.size()) {
        initializeWriteBuffer();
    }

    if ((int) d_writeBuffer.size() < numBytes + d_writeBufferOffset) {
        d_writeBuffer.resize(d_writeBufferOffset + numBytes);
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->write(buffer, numBytes);
        if (s == numBytes) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (0 < s || btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
            if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                s = 0;
            }

            TcpCbChannel_WReg *request =
                new (d_wrequestPool) TcpCbChannel_WReg(numBytes,
                                                     numBytes,
                                                     writeCallback, flags);
            request->d_data.d_s.d_length -= s;
            if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                numBytes - s) {
                d_writeBuffer.resize(d_writeBufferOffset + numBytes - s);
            }
            bsl::memcpy(&d_writeBuffer.front() + d_writeBufferOffset,
                        buffer,
                        numBytes - s);
            d_writeBufferOffset += (numBytes - s);
            d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(numBytes,
                                                         numBytes,
                                                         writeCallback,
                                                         flags);
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                    numBytes) {
                    d_writeBuffer.resize(d_writeBufferOffset + numBytes);
                }
                bsl::memcpy(&d_writeBuffer.front() + d_writeBufferOffset,
                            buffer,
                            numBytes);
                d_writeBufferOffset += numBytes;
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          btlso::EventType::e_WRITE,
                                          d_bufferedWriteFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(numBytes,
                                                 numBytes,
                                                 writeCallback, flags);
        d_writeRequests.push_front(request);
        if ((int) d_writeBuffer.size() < d_writeBufferOffset + numBytes) {
            d_writeBuffer.resize(d_writeBufferOffset + numBytes);
        }

        bsl::memcpy(&d_writeBuffer.front() + d_writeBufferOffset,
                    buffer,
                    numBytes);
        d_writeBufferOffset += numBytes;
    }
    return 0;
}

int TcpCbChannel::bufferedWritev(const btls::Iovec    *buffers,
                                 int                   numBuffers,
                                 const WriteCallback&  writeCallback,
                                 int                   flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        int length = btls::IovecUtil::length(buffers, numBuffers);
        if (s == length) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (0 < s) {
                // Got a partial result -- enqueue the rest
                int idx, offset;
                btls::IovecUtil::pivot(&idx, &offset,
                                      buffers, numBuffers, s);
                BSLS_ASSERT(0 <= idx);
                BSLS_ASSERT(idx < numBuffers);
                BSLS_ASSERT(0 <= offset);
                BSLS_ASSERT(offset < buffers[idx].length());

                int numBytes = length - s;
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                    numBytes) {
                    d_writeBuffer.resize(d_writeBufferOffset + numBytes);
                }

                bsl::memcpy(&d_writeBuffer.front() + d_writeBufferOffset,
                            (char*)(buffers[idx].buffer()) + offset,
                            buffers[idx].length() - offset);

                if (idx < numBuffers - 1) {
                    int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers + idx + 1,
                                  numBuffers - idx - 1);
                    BSLS_ASSERT(length == len);
                }
                d_writeBufferOffset += numBytes;
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                request->d_data.d_s.d_length -= s;
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;

                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          btlso::EventType::e_WRITE,
                                          d_bufferedWriteFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        int length = btls::IovecUtil::length(buffers, numBuffers);
        if ((int) d_writeBuffer.size() < d_writeBufferOffset + length) {
            d_writeBuffer.resize(d_writeBufferOffset + length);
        }
        int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
        BSLS_ASSERT(length == len);

        d_writeBufferOffset += length;

        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                 length,
                                                 writeCallback, flags);

        d_writeRequests.push_front(request);
    }
    return 0;
}

int TcpCbChannel::bufferedWritev(const btls::Ovec     *buffers,
                                 int                   numBuffers,
                                 const WriteCallback&  writeCallback,
                                 int                   flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;                                                    // RETURN
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        int length = btls::IovecUtil::length(buffers, numBuffers);
        if (s == length) {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        else if (0 < s) {

                // Got a partial result -- enqueue the rest
                int idx, offset;
                btls::IovecUtil::pivot(&idx, &offset,
                                      buffers, numBuffers, s);
                BSLS_ASSERT(0 <= idx);
                BSLS_ASSERT(idx < numBuffers);
                BSLS_ASSERT(0 <= offset);
                BSLS_ASSERT(offset < buffers[idx].length());

                int numBytes = length - s;
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                    numBytes) {
                    d_writeBuffer.resize(d_writeBufferOffset + numBytes);
                }

                bsl::memcpy(&d_writeBuffer.front() + d_writeBufferOffset,
                            (char*) const_cast<void *>(buffers[idx].buffer())
                                + offset,
                            buffers[idx].length() - offset);

                if (idx < numBuffers - 1) {
                    int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers + idx + 1,
                                  numBuffers - idx - 1);
                    BSLS_ASSERT(length == len);
                }
                d_writeBufferOffset += numBytes;
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                request->d_data.d_s.d_length -= s;
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == s) {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;
                TcpCbChannel_WReg *request =
                    new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (btlso::SocketHandle::e_ERROR_INTERRUPTED == s) {
            if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
                writeCallback(0, e_INTERRUPT);
                return 0;                                             // RETURN
            }
            else {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;

                TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                        TcpCbChannel_WReg(length, length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;                                                 // RETURN
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          btlso::EventType::e_WRITE,
                                          d_bufferedWriteFunctor)) {
            TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, e_CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;                                                // RETURN
        }
    }
    else {
        int length = btls::IovecUtil::length(buffers, numBuffers);
        if ((int) d_writeBuffer.size() < d_writeBufferOffset + length) {
            d_writeBuffer.resize(d_writeBufferOffset + length);
        }
        int len = btls::IovecUtil::gather(
                                  &d_writeBuffer.front() + d_writeBufferOffset,
                                  static_cast<int>(d_writeBuffer.size())
                                                         - d_writeBufferOffset,
                                  buffers,
                                  numBuffers);
        BSLS_ASSERT(length == len);

        d_writeBufferOffset += length;

        TcpCbChannel_WReg *request =
            new (d_wrequestPool) TcpCbChannel_WReg(length,
                                                 length,
                                                 writeCallback, flags);

        d_writeRequests.push_front(request);
    }
    return 0;
}

///Cancel section
///--------------

void TcpCbChannel::cancelAll() {
    cancelRead();
    cancelWrite();
}

void TcpCbChannel::cancelRead() {
    if (d_currentReadRequest_p) {
        // A callback is active -- can't destroy current request.

        bsl::deque<TcpCbChannel_RReg *> toBeCancelled(
                           d_readRequests.begin(),
                           d_readRequests.begin() + d_readRequests.size() - 1,
                           d_allocator_p);
        d_readRequests.erase(
                           d_readRequests.begin(),
                           d_readRequests.begin() + d_readRequests.size() - 1);
        dequeue(&toBeCancelled, 0, e_DEQUEUED, &d_rrequestPool);
        BSLS_ASSERT(d_currentReadRequest_p == d_readRequests.back());
    }
    else {
        bsl::deque<TcpCbChannel_RReg *>
                                  toBeCancelled(d_readRequests, d_allocator_p);
        d_readRequests.clear();
        int numToCancel = static_cast<int>(toBeCancelled.size());

        if (numToCancel) {
            d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                                btlso::EventType::e_READ);
        }
        dequeue(&toBeCancelled, 0, e_DEQUEUED, &d_rrequestPool);
    }
}

void TcpCbChannel::cancelWrite() {
    if (d_currentWriteRequest_p) {
        // A callback is active -- can't destroy current request.

        bsl::deque<TcpCbChannel_WReg *> toBeCancelled(
                          d_writeRequests.begin(),
                          d_writeRequests.begin() + d_writeRequests.size() - 1,
                          d_allocator_p);

        d_writeRequests.erase(d_writeRequests.begin(),
                              d_writeRequests.begin()
                                                 + d_writeRequests.size() - 1);
        dequeue(&toBeCancelled, 0, e_DEQUEUED, &d_wrequestPool);
        BSLS_ASSERT(d_currentWriteRequest_p == d_writeRequests.back());
    }
    else {
        bsl::deque<TcpCbChannel_WReg *> toBeCancelled(d_writeRequests,
                                                      d_allocator_p);
        d_writeRequests.clear();
        int numToCancel = static_cast<int>(toBeCancelled.size());
        if (numToCancel) {
            d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                                btlso::EventType::e_WRITE);
        }
        dequeue(&toBeCancelled, 0, e_DEQUEUED, &d_wrequestPool);
    }
}

void TcpCbChannel::invalidate() {
    d_isInvalidReadFlag = 1;
    d_isInvalidWriteFlag = 1;
}

void TcpCbChannel::invalidateRead() {
    d_isInvalidReadFlag = 1;
}

void TcpCbChannel::invalidateWrite() {
    d_isInvalidWriteFlag = 1;
}

void TcpCbChannel::setReadEventManager(btlso::TimerEventManager *manager) {
    BSLS_ASSERT(manager);
    BSLS_ASSERT(0 == numPendingReadOperations());
    d_rManager_p = manager;
}

void TcpCbChannel::setWriteEventManager(btlso::TimerEventManager *manager) {
    BSLS_ASSERT(manager);
    BSLS_ASSERT(0 == numPendingWriteOperations());
    d_wManager_p = manager;
}

// ACCESSORS

int TcpCbChannel::isInvalidRead() const {
   return d_isInvalidReadFlag ;
}

int TcpCbChannel::isInvalidWrite() const {
   return d_isInvalidWriteFlag ;
}

int TcpCbChannel::numPendingReadOperations() const {
    return static_cast<int>(d_readRequests.size());
}

int TcpCbChannel::numPendingWriteOperations() const {
    return static_cast<int>(d_writeRequests.size());
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
