// btesos_tcpcbchannel.cpp       -*-C++-*-
#include <btesos_tcpcbchannel.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcpcbchannel_cpp,"$Id$ $CSID$")

#include <bteso_timereventmanager.h>
#include <bteso_streamsocket.h>
#include <btesc_flag.h>
#include <btes_iovecutil.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_cstdio.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // Local typedefs and enums
                       // ========================

typedef btesc_CbChannel::ReadCallback         ReadCb;
typedef btesc_CbChannel::BufferedReadCallback BReadCb;
typedef btesc_CbChannel::WriteCallback        WriteCb;

enum {
    RCALLBACK_SIZE  = sizeof(btesc_CbChannel::ReadCallback),
    BRCALLBACK_SIZE = sizeof(btesc_CbChannel::BufferedReadCallback),
    WCALLBACK_SIZE  = sizeof(btesc_CbChannel::WriteCallback),

    RARENA_SIZE     = RCALLBACK_SIZE < BRCALLBACK_SIZE
                    ? BRCALLBACK_SIZE : RCALLBACK_SIZE,

    ARENA_SIZE      = RARENA_SIZE < WCALLBACK_SIZE
                    ? WCALLBACK_SIZE : RARENA_SIZE
};

enum {
    DEQUEUED  = -1,
    CANCELLED = -1,
    CONNECTION_CLOSED       = -1,
    INTERRUPT =  1
};

                       // ==============================
                       // class btesos_TcpCbChannel_RReg
                       // ==============================

class btesos_TcpCbChannel_RReg {
public:
    enum {
        VFUNC2  = 0,
        VFUNC3  = 1
    };

    enum OperationCategory {
        BUFFERED,      // the request is for a buffered (non-vectored)
                       // operation
        NON_BUFFERED,  // the request is for a non-buffered (non-vectored)
                       // operation
        VECTORED_I     // the request is for a vectored operation
    };

    union {
        char                               d_arena[ARENA_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;  // for alignment
    } d_cb;

    // Data for the I/O operation
    union {
        struct {
            char              *d_buffer;  // the buffer for the next operation
            int                d_length;  // the length of the request for an
                                          // operation
        } d_s;
        struct {
            const btes_Iovec *d_buffers;
            int                      d_numBuffers;
        } d_vi;
    } d_data;
    int               d_requestLength;    // Original length requested.

    OperationCategory d_category;         // Operation category

    int               d_numSysCalls;

    int               d_flags;            // flags supplied in request.
    char              d_callbackType;

    // CREATORS
    btesos_TcpCbChannel_RReg(char *buffer, int length,
                             int numSysCalls,
                             const ReadCb& callback,
                             int flags);

    btesos_TcpCbChannel_RReg(const btes_Iovec *buffers,
                             int numBuffers,
                             int numSysCalls,
                             const ReadCb& callback,
                             int flags);

    btesos_TcpCbChannel_RReg(int length,
                             int numSysCalls,
                             const BReadCb& callback,
                             int flags);

    ~btesos_TcpCbChannel_RReg();
        // Destroy this request.

    // MANIPULATORS
    void invoke(const char *buffer, int status, int augStatus) const;
        // Invoke the callback contained in this request passing it the
        // specified 'buffer', the specified 'status', and the specified
        // 'augStatus'.  The behavior is undefined unless the callback type (
        // as reported by 'd_callbackType') is 'VFUNC3'.

    void invoke(int status, int augStatus) const;
        // Invoke the callback contained in this request passing it the
        // specified 'status', and the specified 'augStatus'.  The behavior
        // is undefined unless the callback type (as reported by
        // 'd_callbackType') is 'VFUNC2'.

    void invokeConditionally(int status, int augStatus) const;
        // Invoke the callback contained in this requests passing it the
        // specified 'status' and the specified 'augStatus'.  If the
        // contained callback is VFUNC3, NULL address is passed for
        // the first argument (i.e., buffer).

};

// CREATORS

inline
btesos_TcpCbChannel_RReg::btesos_TcpCbChannel_RReg(
        int            length,
        int            numSysCalls,
        const BReadCb& callback,
        int            flags)
: d_requestLength(length)
, d_category(BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(VFUNC3) // Buffered
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);

    d_data.d_s.d_buffer = NULL;
    d_data.d_s.d_length = length;
    new (d_cb.d_arena) BReadCb(callback);
}

inline
btesos_TcpCbChannel_RReg::btesos_TcpCbChannel_RReg(
        const btes_Iovec *buffers,
        int                      numBuffers,
        int                      numSysCalls,
        const ReadCb&            callback,
        int                      flags)
: d_requestLength(btes_IovecUtil::length(buffers, numBuffers))
, d_category(VECTORED_I)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(VFUNC2)
{
    BSLS_ASSERT(0 < numBuffers);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffers);

    d_data.d_vi.d_buffers = buffers;
    d_data.d_vi.d_numBuffers = numBuffers;
    new (d_cb.d_arena) ReadCb(callback);
}

inline
btesos_TcpCbChannel_RReg::btesos_TcpCbChannel_RReg(
        char          *buffer,
        int            length,
        int            numSysCalls,
        const ReadCb&  callback,
        int            flags)
: d_requestLength(length)
, d_category(NON_BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
, d_callbackType(VFUNC2) // Buffered
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);
    BSLS_ASSERT(buffer);

    d_data.d_s.d_buffer = buffer;
    d_data.d_s.d_length = length;
    new (d_cb.d_arena) ReadCb(callback);
}

inline
btesos_TcpCbChannel_RReg::~btesos_TcpCbChannel_RReg() {
    if (d_callbackType == VFUNC3) {
        bdef_Function<void (*)(const char *, int, int)> *cb =
                    (bdef_Function<void (*)(const char *, int, int)> *)
                        (void *)d_cb.d_arena;

        bslalg_ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        BSLS_ASSERT(d_callbackType == VFUNC2);
        bdef_Function<void (*)(int, int)> *cb =
            (bdef_Function<void (*)(int, int)> *) (void *) d_cb.d_arena;

        bslalg_ScalarDestructionPrimitives::destroy(cb);
    }
}

// MANIPULATORS

inline
void btesos_TcpCbChannel_RReg::invoke(int status, int asyncStatus) const {
    BSLS_ASSERT(VFUNC2 == d_callbackType);
    bdef_Function<void (*)(int, int)> *cb =
        (bdef_Function<void (*)(int, int)> *) (void *) d_cb.d_arena;
    (*cb)(status, asyncStatus);
}

inline
void btesos_TcpCbChannel_RReg::invoke(const char *buffer,
                                      int status, int asyncStatus) const {
    BSLS_ASSERT(VFUNC3 == d_callbackType);
    BReadCb *cb = (BReadCb *)(void *)d_cb.d_arena;
    (*cb)(buffer, status, asyncStatus);
}

inline
void btesos_TcpCbChannel_RReg::invokeConditionally(int status,
                                                   int asyncStatus) const {
    if (VFUNC2 == d_callbackType) {
        invoke(status, asyncStatus);
    }
    else {
        invoke(NULL, status, asyncStatus);
    }
}

bsl::ostream& operator<<(bsl::ostream&                   out,
                         const btesos_TcpCbChannel_RReg& reg) {
    switch(reg.d_category) {
       case btesos_TcpCbChannel_RReg::BUFFERED: {
           out << "B" << reg.d_requestLength << ", "
               << reg.d_data.d_s.d_length << ';' << bsl::flush;
       } break;
       case btesos_TcpCbChannel_RReg::NON_BUFFERED: {
           out << "N" << reg.d_requestLength << ", "
               << reg.d_data.d_s.d_length << ';' << bsl::flush;
       } break;
       case btesos_TcpCbChannel_RReg::VECTORED_I: {
           out << "V" << reg.d_requestLength << ", "
               << reg.d_data.d_vi.d_numBuffers << ';' << bsl::flush;
       } break;
    }
    return out;
}

                       // ==============================
                       // class btesos_TcpCbChannel_WReg
                       // ==============================

class btesos_TcpCbChannel_WReg {
public:
    enum OperationCategory {
        BUFFERED,      // the request is for a buffered (non-vectored)
                       // operation
        NON_BUFFERED,  // the request is for a non-buffered (non-vectored)
                       // operation
        VECTORED_I,    // the request is for a vectored read or write operation
        VECTORED_O     // the request is for a vectored write operation
    };

    bdef_Function<void (*)(int, int)> d_callback;

    // Data for the I/O operation
    union {
        struct {
            const char        *d_buffer;  // the buffer for the next operation
            int                d_length;  // the length of the request for an
                                          // operation
        } d_s;
        struct {
            const btes_Iovec *d_buffers;
            int                      d_numBuffers;
        } d_vi;
        struct {
            const btes_Ovec  *d_buffers;
            int                      d_numBuffers;
        } d_vo;
    } d_data;
    int               d_requestLength;    // Original length requested.
    OperationCategory d_category;         // Operation category

    int               d_numSysCalls;

    int               d_flags;       // flags supplied in request.

    // CREATORS
    btesos_TcpCbChannel_WReg(const char *buffer, int length,
                             int numSysCalls,
                             const WriteCb& callback,
                             int flags);

    btesos_TcpCbChannel_WReg(const btes_Iovec *buffers,
                             int numBuffers,
                             int numSysCalls,
                             const WriteCb& callback,
                             int flags);

    btesos_TcpCbChannel_WReg(const btes_Ovec *buffers,
                             int numBuffers,
                             int numSysCalls,
                             const WriteCb& callback,
                             int flags);

    btesos_TcpCbChannel_WReg(int length,
                             int numSysCalls,
                             const WriteCb& callback,
                             int flags);

    ~btesos_TcpCbChannel_WReg();
        // Destroy this request.

    // MANIPULATORS
    void invoke(int, int) const;
    void invokeConditionally(int, int) const;
        // Invoke the callback contained in this request passing it the
        // specified 'status', and the specified 'augStatus'.
};

// CREATORS

inline
btesos_TcpCbChannel_WReg::btesos_TcpCbChannel_WReg(
        int            length,
        int            numSysCalls,
        const WriteCb& callback,
        int            flags)
: d_callback(callback)
, d_requestLength(length)
, d_category(BUFFERED)
, d_numSysCalls(numSysCalls)
, d_flags(flags)
{
    BSLS_ASSERT(0 < length);
    BSLS_ASSERT(0 < numSysCalls);

    d_data.d_s.d_buffer = NULL;
    d_data.d_s.d_length = length;
}

inline
btesos_TcpCbChannel_WReg::btesos_TcpCbChannel_WReg(
        const btes_Iovec *buffers,
        int                      numBuffers,
        int                      numSysCalls,
        const WriteCb&           callback,
        int                      flags)
: d_callback(callback)
, d_requestLength(btes_IovecUtil::length(buffers, numBuffers))
, d_category(VECTORED_I)
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
btesos_TcpCbChannel_WReg::btesos_TcpCbChannel_WReg(
        const btes_Ovec  *buffers,
        int                      numBuffers,
        int                      numSysCalls,
        const WriteCb&           callback,
        int                      flags)
: d_callback(callback)
, d_requestLength(btes_IovecUtil::length(buffers, numBuffers))
, d_category(VECTORED_O)
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
btesos_TcpCbChannel_WReg::btesos_TcpCbChannel_WReg(
        const char    *buffer,
        int            length,
        int            numSysCalls,
        const WriteCb& callback,
        int            flags)
: d_callback(callback)
, d_requestLength(length)
, d_category(NON_BUFFERED)
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
btesos_TcpCbChannel_WReg::~btesos_TcpCbChannel_WReg() {
    // The callback will be destroyed on its own
}

// MANIPULATORS

inline
void btesos_TcpCbChannel_WReg::invoke(int status, int asyncStatus) const
{
    d_callback(status, asyncStatus);
}

inline
void btesos_TcpCbChannel_WReg::invokeConditionally(int status,
                                                   int asyncStatus) const
{
    d_callback(status, asyncStatus);
}

                       // ================================
                       // local function completeOperation
                       // ================================
static inline
int completeOperation(btesos_TcpCbChannel_RReg *request,
                      bsl::vector<char>        *bufferPtr,
                      int                      *offset,
                      int                      *SP)
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
        return 0; // No data in buffer.
    }

    char *buffer = &bufferPtr->front();
    BSLS_ASSERT(buffer && 0 < *offset);
    int rv = 0;

    switch(request->d_category) {
      case btesos_TcpCbChannel_RReg::BUFFERED: {
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
      case btesos_TcpCbChannel_RReg::NON_BUFFERED: {
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
      case btesos_TcpCbChannel_RReg::VECTORED_I: {
          int s = btes_IovecUtil::scatter(request->d_data.d_vi.d_buffers,
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
dequeue(bsl::deque<REG *> *queue, int status, int augStatus, bdema_Pool *pool)
    // Invoke each callback contained in the specified 'queue' with the
    // specified 'status' and the specified 'augStatus' in the reverse order,
    // remove each request from the queue and deallocate memory associated with
    // the request using the specified 'pool'.  The behavior is undefined if
    // either 'pool' or 'queue' is 0.
{
    BSLS_ASSERT(queue);
    int numElements = queue->size();

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
void initializeBuffer(bsl::vector<char>                     *buffer,
                      bteso_StreamSocket<bteso_IPv4Address> *socket_p,
                      int                                    option)
{
    enum { DEFAULT_BUFFER_SIZE = 8192 };
    int result;
    int s = socket_p->socketOption(&result,
                                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                   option);
    if (!s) {
        BSLS_ASSERT(0 < result);
        buffer->resize(result);
    }
    else {
        buffer->resize(DEFAULT_BUFFER_SIZE);
    }
}

// ============================================================================
//                        END LOCAL DEFINITIONS
// ============================================================================

                          // -------------------------
                          // class btesos_TcpCbChannel
                          // -------------------------

// PRIVATE MANIPULATORS

void btesos_TcpCbChannel::initializeReadBuffer(int size)
{
    if (size > 0) {
        d_readBuffer.resize(size);
    }
    else {
        initializeBuffer(&d_readBuffer,  d_socket_p,
                         bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
    }
}

void btesos_TcpCbChannel::initializeWriteBuffer(int size)
{
    if (size > 0) {
        d_writeBuffer.resize(size);
    }
    else {
        initializeBuffer(&d_writeBuffer,  d_socket_p,
                         bteso_SocketOptUtil::BTESO_SENDBUFFER);
    }
}

void btesos_TcpCbChannel::bufferedReadCb()
{
    BSLS_ASSERT(d_socket_p);
    BSLS_ASSERT(d_readRequests.size());
    BSLS_ASSERT(0 == d_currentReadRequest_p);
    BSLS_ASSERT(0 == d_readBufferSP);
    d_currentReadRequest_p = d_readRequests.back();
    BSLS_ASSERT(d_currentReadRequest_p);
    BSLS_ASSERT(btesos_TcpCbChannel_RReg::BUFFERED ==
           d_currentReadRequest_p->d_category);
    BSLS_ASSERT(btesos_TcpCbChannel_RReg::VFUNC3 ==
           d_currentReadRequest_p->d_callbackType);

    if (0 == d_readBuffer.size()) {
        initializeReadBuffer();
    }

    int requestLength = d_currentReadRequest_p->d_requestLength;
    int numBytesRemaining = requestLength - d_readBufferOffset;
    int available = d_readBuffer.size() - d_readBufferOffset;
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
            return;
        }
    }
    else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
        // Connection was closed
        invalidateRead();
        d_currentReadRequest_p->invoke(NULL, CONNECTION_CLOSED, 0);
    } else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
        if (d_currentReadRequest_p->d_flags
                                        & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            bsl::deque<btesos_TcpCbChannel_RReg *>
                                 toBeDispatched(d_readRequests, d_allocator_p);
            toBeDispatched.pop_back();
            d_readRequests.erase(d_readRequests.begin(),
                                 d_readRequests.begin() +
                                                    d_readRequests.size() - 1);
            d_currentReadRequest_p->invoke(&d_readBuffer.front(),
                                           d_readBufferOffset, INTERRUPT);

            dequeue(&toBeDispatched, 0, DEQUEUED, &d_rrequestPool);
        }
        else {
            // Restart on the interrupt.
            d_currentReadRequest_p = NULL;
            return;
        }
    }
    else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
        d_currentReadRequest_p = NULL;
        return; // Fake wake up from the event manager
                // The number of system calls is not counted
    }
    else {
        BSLS_ASSERT(s < 0);
        BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
        BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
        invalidateRead(); // Hard error on channel.
        d_currentReadRequest_p->invoke(NULL, s - 1, 0);
        d_readRequests.pop_back();

        dequeue(&d_readRequests, 0, DEQUEUED, &d_rrequestPool);
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
        while (btesos_TcpCbChannel_RReg::BUFFERED !=
                                       d_currentReadRequest_p->d_category) {
            if (0 !=
                 d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   d_readFunctor)) {
                d_currentReadRequest_p->invoke(0, CANCELLED);
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
                                                  bteso_EventType::BTESO_READ);
                break;
            }
        }
    }
    else {
        d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                           bteso_EventType::BTESO_READ);
    }

    // Invariant must be maintained.
    d_currentReadRequest_p = NULL;
}

void btesos_TcpCbChannel::readCb()
{
    BSLS_ASSERT(d_readRequests.size());
    BSLS_ASSERT(d_currentReadRequest_p == NULL);
    d_currentReadRequest_p = d_readRequests.back();
    BSLS_ASSERT(d_currentReadRequest_p);

    switch(d_currentReadRequest_p->d_category) {
      case btesos_TcpCbChannel_RReg::NON_BUFFERED:
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
                  return;
              }
          }
          else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
              // Connection was closed
              invalidateRead();
              d_currentReadRequest_p->invoke(CONNECTION_CLOSED, 0);
              break;
          } else if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
              if (d_currentReadRequest_p->d_flags &
                  btesc_Flag::BTESC_ASYNC_INTERRUPT)
              {
                  d_currentReadRequest_p->invoke(requestLength - numBytes,
                                                 INTERRUPT);
              }
              else {
                  d_currentReadRequest_p = NULL;
                  return;
              }
          }
          else if (s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentReadRequest_p = NULL;
              return;
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
              d_currentReadRequest_p->invoke(s - 1, 0);
          }
      } break;
      case btesos_TcpCbChannel_RReg::VECTORED_I: {
          const btes_Iovec *buffers =
              d_currentReadRequest_p->d_data.d_vi.d_buffers;
          BSLS_ASSERT(buffers);

          int numBuffers = d_currentReadRequest_p->d_data.d_vi.d_numBuffers;
          int requestLength = d_currentReadRequest_p->d_requestLength;
          int numBytes = btes_IovecUtil::length(buffers, numBuffers);

          int s = d_socket_p->readv(buffers, numBuffers);

          if (0 < s) {
              --d_currentReadRequest_p->d_numSysCalls;
              numBytes -= s;
              BSLS_ASSERT(0 <= numBytes);
              // Note: we support only RAW VECTORIZED operations for
              // performance reasons.  If a vectorized request is not
              // satisfied completely in one read, the user of the
              // channel can do adjustments more efficiently.

              BSLS_ASSERT(0 == d_currentReadRequest_p->d_numSysCalls);
              d_currentReadRequest_p->invoke(requestLength - numBytes, 0);
          }
          else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
              // Connection was closed
              invalidateRead();
              d_currentReadRequest_p->invoke(CONNECTION_CLOSED, 0);
              break;
          } else if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
              if (d_currentReadRequest_p->d_flags &
                  btesc_Flag::BTESC_ASYNC_INTERRUPT)
              {
                  d_currentReadRequest_p->invoke(requestLength - numBytes,
                                                 INTERRUPT);
              }
              else {
                  d_currentReadRequest_p = NULL;
                  return;
              }
          }
          else if (s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentReadRequest_p = NULL;
              return;
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

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

        while (btesos_TcpCbChannel_RReg::BUFFERED ==
                                      d_currentReadRequest_p->d_category)
        {
            if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   d_bufferedReadFunctor))
            {
                d_currentReadRequest_p->invoke(NULL, 0, CANCELLED);
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
                d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                                  bteso_EventType::BTESO_READ);
                break;
            }
        }
    }
    else {
        d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                            bteso_EventType::BTESO_READ);
    }

    // Invariant must be maintained.
    d_currentReadRequest_p = NULL;
}

void btesos_TcpCbChannel::bufferedWriteCb() {
    BSLS_ASSERT(d_socket_p);
    BSLS_ASSERT(d_writeRequests.size());
    BSLS_ASSERT(btesos_TcpCbChannel_WReg::BUFFERED ==
                                           d_writeRequests.back()->d_category);

    int numBytesToWrite = 0;
    int numPendingRequests = d_writeRequests.size();
    for (int i = numPendingRequests - 1; 0 <= i; --i) {
        if (d_writeRequests[i]->d_category
                                       != btesos_TcpCbChannel_WReg::BUFFERED) {
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

            while (btesos_TcpCbChannel_WReg::BUFFERED !=
                                         d_currentWriteRequest_p->d_category)
            {
                if (0 != d_wManager_p->registerSocketEvent(
                                                 d_socket_p->handle(),
                                                 bteso_EventType::BTESO_WRITE,
                                                 d_writeFunctor))
                {
                    d_currentWriteRequest_p->invoke(0, CANCELLED);
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
                                                 bteso_EventType::BTESO_WRITE);
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
                                               bteso_EventType::BTESO_WRITE);
        }
        d_currentWriteRequest_p = NULL;
        return;
    }
    else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
        // Connection was closed
        invalidateWrite();
        d_currentWriteRequest_p->invoke(CONNECTION_CLOSED, 0);

    } else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
        if (d_currentWriteRequest_p->d_flags
                                         & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            bsl::deque<btesos_TcpCbChannel_WReg *>
                                toBeDispatched(d_writeRequests, d_allocator_p);
            toBeDispatched.pop_back();
            d_writeRequests.erase(d_writeRequests.begin(),
                                  d_writeRequests.begin() +
                                                   d_writeRequests.size() - 1);
            d_currentWriteRequest_p->invoke(
                    d_currentWriteRequest_p->d_requestLength -
                    d_currentWriteRequest_p->d_data.d_s.d_length,
                    INTERRUPT);

            dequeue(&toBeDispatched, 0, DEQUEUED, &d_wrequestPool);
        }
        else {
            // Restart on the interrupt.
            d_currentWriteRequest_p = NULL;
            return;
        }
    }
    else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
        d_currentWriteRequest_p = NULL;
        return; // Fake wake up from the event manager
                // The number of system calls is not counted
    }
    else {
        // Hard error on the channel -> invalidate and dequeue.
        BSLS_ASSERT(s < 0);
        BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
        BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
        invalidateWrite();
        d_writeRequests.pop_back();
        d_currentWriteRequest_p->invoke(s - 1, 0);
        dequeue(&d_writeRequests, 0, DEQUEUED, &d_wrequestPool);
        d_writeRequests.push_back(d_currentWriteRequest_p);
    }

    d_writeRequests.pop_back();
    d_wrequestPool.deleteObjectRaw(d_currentWriteRequest_p);
    d_currentWriteRequest_p = NULL;

    if (d_writeRequests.size()) {
        while (btesos_TcpCbChannel_WReg::BUFFERED !=
                                      d_currentWriteRequest_p->d_category)
        {
            if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                             bteso_EventType::BTESO_WRITE,
                                             d_writeFunctor))
            {
                d_currentWriteRequest_p->invoke(0, CANCELLED);
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
                                                 bteso_EventType::BTESO_WRITE);
                break;
            }
        }
    }
    else {
        d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                           bteso_EventType::BTESO_WRITE);
    }

    // Invariant must be maintained.
    d_currentWriteRequest_p = NULL;
}

void btesos_TcpCbChannel::writeCb() {
    // There is space in the socket's system buffer for data.
    BSLS_ASSERT(d_writeRequests.size());
    BSLS_ASSERT(NULL == d_currentWriteRequest_p);
    d_currentWriteRequest_p = d_writeRequests.back();
    BSLS_ASSERT(d_currentWriteRequest_p);

    switch(d_currentWriteRequest_p->d_category) {
      case btesos_TcpCbChannel_WReg::NON_BUFFERED: {
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
                  return; // Do nothing - operation is incomplete.
              }
          }
          else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(CONNECTION_CLOSED, 0);
              break;
          } else if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btesc_Flag::BTESC_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(requestLength - numBytes,
                                                  INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;
              }
          }
          else if (s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
              d_currentWriteRequest_p->invoke(s - 1, 0);
          }
      } break;

      case btesos_TcpCbChannel_WReg::VECTORED_I: {
          const btes_Iovec *buffers =
              d_currentWriteRequest_p->d_data.d_vi.d_buffers;
          int numBuffers = d_currentWriteRequest_p->d_data.d_vi.d_numBuffers;

          int s = d_socket_p->writev(buffers, numBuffers);
          if (0 < s) {
              // Wrote 's' bytes into the socket.
              --d_currentWriteRequest_p->d_numSysCalls;

              d_currentWriteRequest_p->invoke(s, 0);
          }
          else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(CONNECTION_CLOSED, 0);
              break;
          } else if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btesc_Flag::BTESC_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(0,
                                                  INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;
              }
          }
          else if (s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

              d_currentWriteRequest_p->invoke(s - 1, 0);
          }
      } break;

      case btesos_TcpCbChannel_WReg::VECTORED_O: {
          const btes_Ovec *buffers =
              d_currentWriteRequest_p->d_data.d_vo.d_buffers;
          int numBuffers = d_currentWriteRequest_p->d_data.d_vo.d_numBuffers;

          int s = d_socket_p->writev(buffers, numBuffers);
          if (0 < s) {
              // Wrote 's' bytes into the socket.
              --d_currentWriteRequest_p->d_numSysCalls;
              d_currentWriteRequest_p->invoke(s, 0);
          }
          else if (bteso_SocketHandle::BTESO_ERROR_EOF == s) {
              // Connection was closed
              invalidateWrite();
              d_currentWriteRequest_p->invoke(CONNECTION_CLOSED, 0);
              break;
          } else if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
              if (d_currentWriteRequest_p->d_flags &
                  btesc_Flag::BTESC_ASYNC_INTERRUPT)
              {
                  d_currentWriteRequest_p->invoke(0, INTERRUPT);
              }
              else {
                  d_currentWriteRequest_p = NULL;
                  return;
              }
          }
          else if (s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
              // Fake wake up from the event manager.
              d_currentWriteRequest_p = NULL;
              return;
          }
          else {
              BSLS_ASSERT(s < 0);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);
              BSLS_ASSERT(s != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

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
        while ((int) btesos_TcpCbChannel_WReg::BUFFERED ==
               (int) d_currentWriteRequest_p->d_category)
        {   // will replace the current event.
            if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                              bteso_EventType::BTESO_WRITE,
                                              d_bufferedWriteFunctor))
            {
                d_currentWriteRequest_p->invoke(0, CANCELLED);
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
                                                 bteso_EventType::BTESO_WRITE);
                break;
            }
        }
    }
    else {
        d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                            bteso_EventType::BTESO_WRITE);

    }

    d_currentWriteRequest_p = NULL;
    BSLS_ASSERT(NULL == d_currentWriteRequest_p);
}

// CREATORS

btesos_TcpCbChannel::btesos_TcpCbChannel(
        bteso_StreamSocket<bteso_IPv4Address> *sSocket,
        bteso_TimerEventManager               *manager,
        bslma_Allocator                       *basicAllocator)
: d_socket_p(sSocket)
, d_rManager_p(manager)
, d_wManager_p(manager)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferSP(0)
, d_writeBuffer(basicAllocator)
, d_writeBufferOffset(0)
, d_rrequestPool(sizeof(btesos_TcpCbChannel_RReg), basicAllocator)
, d_wrequestPool(sizeof(btesos_TcpCbChannel_WReg), basicAllocator)
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
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::bufferedReadCb, this)
            , d_allocator_p);

    d_readFunctor
        = bdef_Function<void (*)()>(
               bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::readCb, this)
             , d_allocator_p);

    d_bufferedWriteFunctor
        = bdef_Function<void (*)()>(
             bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::bufferedWriteCb, this)
           , d_allocator_p);

    d_writeFunctor
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::writeCb, this)
            , d_allocator_p);

    d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
}

btesos_TcpCbChannel::btesos_TcpCbChannel(
        bteso_StreamSocket<bteso_IPv4Address> *sSocket,
        bteso_TimerEventManager               *rManager,
        bteso_TimerEventManager               *wManager,
        bslma_Allocator                       *basicAllocator)
: d_socket_p(sSocket)
, d_rManager_p(rManager)
, d_wManager_p(wManager)
, d_readBuffer(basicAllocator)
, d_readBufferOffset(0)
, d_readBufferSP(0)
, d_writeBuffer(basicAllocator)
, d_writeBufferOffset(0)
, d_rrequestPool(sizeof(btesos_TcpCbChannel_RReg), basicAllocator)
, d_wrequestPool(sizeof(btesos_TcpCbChannel_WReg), basicAllocator)
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
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::bufferedReadCb, this)
            , d_allocator_p);

    d_readFunctor
        = bdef_Function<void (*)()>(
               bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::readCb, this)
             , d_allocator_p);

    d_bufferedWriteFunctor
        = bdef_Function<void (*)()>(
             bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::bufferedWriteCb, this)
           , d_allocator_p);

    d_writeFunctor
        = bdef_Function<void (*)()>(
              bdef_MemFnUtil::memFn(&btesos_TcpCbChannel::writeCb, this)
            , d_allocator_p);

    d_socket_p->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
}

btesos_TcpCbChannel::~btesos_TcpCbChannel() {
    invalidate();
    BSLS_ASSERT(d_currentWriteRequest_p == NULL);
    BSLS_ASSERT(d_currentReadRequest_p == NULL);
    cancelAll(); // Cancel all will deregister it.
}

// MANIPULATORS

///Read section
///------------

int btesos_TcpCbChannel::read(char *buffer, int    numBytes,
                              const ReadCallback&  readCallback,
                              int   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;
    }

    btesos_TcpCbChannel_RReg *request =
        new (d_rrequestPool) btesos_TcpCbChannel_RReg (buffer, numBytes,
                                                  numBytes,
                                                  readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   d_readFunctor)) {

            request->invoke(0, CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;
        }

    }

    d_readRequests.push_front(request);
    return 0;
}

int btesos_TcpCbChannel::readRaw(char                *buffer,
                                      int                  numBytes,
                                      const ReadCallback&  readCallback,
                                      int                  flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;
    }

    btesos_TcpCbChannel_RReg *request =
        new (d_rrequestPool) btesos_TcpCbChannel_RReg (buffer, numBytes,
                                                  1,
                                                  readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   d_readFunctor)) {

            request->invoke(0, CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }

    d_readRequests.push_front(request);
    return 0;
}

int btesos_TcpCbChannel::readv(const btes_Iovec *buffers,
                                    int                      numBuffers,
                                    const ReadCallback&      readCallback,
                                    int                      flags)
{
    return readvRaw(buffers, numBuffers, readCallback, flags);
}

int
btesos_TcpCbChannel::readvRaw(const btes_Iovec *buffers,
                                   int                      numBuffers,
                                   const ReadCallback&      readCallback,
                                   int                      flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidReadFlag) {
        return -1;
    }

    btesos_TcpCbChannel_RReg *request =
        new (d_rrequestPool) btesos_TcpCbChannel_RReg (buffers, numBuffers,
                                                  1, readCallback, flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                         bteso_EventType::BTESO_READ,
                                         d_readFunctor)){

            request->invoke(0, CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;
        }

    }
    d_readRequests.push_front(request);
    return 0;
}

int
btesos_TcpCbChannel::bufferedRead(
        int                         numBytes,
        const BufferedReadCallback& bufferedReadCallback,
        int                         flags)
{
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;
    }

    btesos_TcpCbChannel_RReg *request =
        new (d_rrequestPool) btesos_TcpCbChannel_RReg (numBytes, numBytes,
                                              bufferedReadCallback,
                                              flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request, &d_readBuffer,
                              &d_readBufferOffset, &d_readBufferSP)) {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                         bteso_EventType::BTESO_READ,
                                         d_bufferedReadFunctor)){

            request->invoke(0, 0, CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;
        }

    }

    d_readRequests.push_front(request);
    return 0;
}

int btesos_TcpCbChannel::bufferedReadRaw(
        int                         numBytes,
        const BufferedReadCallback& bufferedReadCallback,
        int                         flags)
{
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidReadFlag) {
        return -1;
    }

    btesos_TcpCbChannel_RReg *request =
        new (d_rrequestPool) btesos_TcpCbChannel_RReg(numBytes, 1,
                                                      bufferedReadCallback,
                                                      flags);

    if (0 == d_readRequests.size()) {
        if (completeOperation(request,
                              &d_readBuffer,
                              &d_readBufferOffset,
                              &d_readBufferSP))
        {
            d_rrequestPool.deleteObjectRaw(request);
            return 0;
        }

        if (0 != d_rManager_p->registerSocketEvent(d_socket_p->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   d_bufferedReadFunctor))
        {
            request->invoke(0, 0, CANCELLED);
            d_rrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }

    d_readRequests.push_front(request);
    return 0;
}

///Write section
///-------------

int btesos_TcpCbChannel::write(const char           *buffer,
                               int                   numBytes,
                               const WriteCallback&  writeCallback,
                               int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->write(buffer, numBytes);

        if (s == numBytes) {
            writeCallback(s, 0);
            return 0;
        }
        else if (0 < s) {
            // Partial result.
            btesos_TcpCbChannel_WReg *request =
                new (d_wrequestPool)
                    btesos_TcpCbChannel_WReg(buffer + s, numBytes,
                                                         numBytes,
                                                         writeCallback, flags);
            request->d_data.d_s.d_length -= s;
            d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                              numBytes,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                              numBytes,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                         bteso_EventType::BTESO_WRITE,
                                         d_writeFunctor)){
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool)
                     btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                      numBytes,
                                                      writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int btesos_TcpCbChannel::writeRaw(const char           *buffer,
                                  int                   numBytes,
                                  const WriteCallback&  writeCallback,
                                  int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->write(buffer, numBytes);
        if (0 < s) {
            writeCallback(s, 0);
            return 0;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                              1,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                             btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                              1,
                                                              writeCallback,
                                                              flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                         bteso_EventType::BTESO_WRITE,
                                         d_writeFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(buffer, numBytes,
                                                      1,
                                                      writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int btesos_TcpCbChannel::writev(const btes_Ovec         *buffers,
                                int                      numBuffers,
                                const WriteCallback&     writeCallback,
                                int                      flags)
{
    return writevRaw(buffers, numBuffers, writeCallback, flags);
}

int btesos_TcpCbChannel::writev(const btes_Iovec        *buffers,
                                int                      numBuffers,
                                const WriteCallback&     writeCallback,
                                int                      flags)
{
    return writevRaw(buffers, numBuffers, writeCallback, flags);
}

int btesos_TcpCbChannel::writevRaw(const btes_Ovec         *buffers,
                                   int                      numBuffers,
                                   const WriteCallback&     writeCallback,
                                   int                      flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        if (0 < s) {
            writeCallback(s, 0);
            return 0;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          bteso_EventType::BTESO_WRITE,
                                          d_writeFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                 1,
                                                 writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int btesos_TcpCbChannel::writevRaw(const btes_Iovec        *buffers,
                                   int                      numBuffers,
                                   const WriteCallback&     writeCallback,
                                   int                      flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);

        if (0 < s) {
            writeCallback(s, 0);
            return 0;
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                       btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                         1,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          bteso_EventType::BTESO_WRITE,
                                          d_writeFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(buffers, numBuffers,
                                                 1,
                                                 writeCallback, flags);
        d_writeRequests.push_front(request);
    }
    return 0;
}

int btesos_TcpCbChannel::bufferedWrite(
        const char           *buffer,
        int                   numBytes,
        const WriteCallback&  writeCallback,
        int                   flags)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < numBytes);

    if (d_isInvalidWriteFlag) {
        return -1;
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
            return 0;
        }
        else if (0 < s || bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
            if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                s = 0;
            }

            btesos_TcpCbChannel_WReg *request =
                new (d_wrequestPool) btesos_TcpCbChannel_WReg(numBytes,
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
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(numBytes,
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
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          bteso_EventType::BTESO_WRITE,
                                          d_bufferedWriteFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(numBytes,
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

int btesos_TcpCbChannel::bufferedWritev(
        const btes_Iovec        *buffers,
        int                      numBuffers,
        const WriteCallback&     writeCallback,
        int                      flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        int length = btes_IovecUtil::length(buffers, numBuffers);
        if (s == length) {
            writeCallback(s, 0);
            return 0;
        }
        else if (0 < s) {
                // Got a partial result -- enqueue the rest
                int idx, offset;
                btes_IovecUtil::pivot(&idx, &offset,
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
                    int len = btes_IovecUtil::gather(
                         &d_writeBuffer.front() + d_writeBufferOffset,
                         d_writeBuffer.size() - d_writeBufferOffset,
                         buffers + idx + 1, numBuffers - idx - 1);
                    BSLS_ASSERT(length == len);
                }
                d_writeBufferOffset += numBytes;
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                request->d_data.d_s.d_length -= s;
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btes_IovecUtil::gather(
                    &d_writeBuffer.front() + d_writeBufferOffset,
                    d_writeBuffer.size() - d_writeBufferOffset,
                    buffers, numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btes_IovecUtil::gather(
                    &d_writeBuffer.front() + d_writeBufferOffset,
                    d_writeBuffer.size() - d_writeBufferOffset,
                    buffers, numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;

                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          bteso_EventType::BTESO_WRITE,
                                          d_bufferedWriteFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        int length = btes_IovecUtil::length(buffers, numBuffers);
        if ((int) d_writeBuffer.size() < d_writeBufferOffset + length) {
            d_writeBuffer.resize(d_writeBufferOffset + length);
        }
        int len = btes_IovecUtil::gather(
            &d_writeBuffer.front() + d_writeBufferOffset,
            d_writeBuffer.size() - d_writeBufferOffset,
            buffers, numBuffers);
        BSLS_ASSERT(length == len);

        d_writeBufferOffset += length;

        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                 length,
                                                 writeCallback, flags);

        d_writeRequests.push_front(request);
    }
    return 0;
}

int btesos_TcpCbChannel::bufferedWritev(
        const btes_Ovec         *buffers,
        int                      numBuffers,
        const WriteCallback&     writeCallback,
        int                      flags)
{
    BSLS_ASSERT(buffers);
    BSLS_ASSERT(0 < numBuffers);

    if (d_isInvalidWriteFlag) {
        return -1;
    }

    if (0 == d_writeRequests.size()) {
        BSLS_ASSERT(0 == d_writeBufferOffset);
        int s = d_socket_p->writev(buffers, numBuffers);
        int length = btes_IovecUtil::length(buffers, numBuffers);
        if (s == length) {
            writeCallback(s, 0);
            return 0;
        }
        else if (0 < s) {

                // Got a partial result -- enqueue the rest
                int idx, offset;
                btes_IovecUtil::pivot(&idx, &offset,
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
                    int len = btes_IovecUtil::gather(
                         &d_writeBuffer.front() + d_writeBufferOffset,
                         d_writeBuffer.size() - d_writeBufferOffset,
                         buffers + idx + 1, numBuffers - idx - 1);
                    BSLS_ASSERT(length == len);
                }
                d_writeBufferOffset += numBytes;
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                request->d_data.d_s.d_length -= s;
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btes_IovecUtil::gather(
                    &d_writeBuffer.front() + d_writeBufferOffset,
                    d_writeBuffer.size() - d_writeBufferOffset,
                    buffers, numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;
                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                         length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
        }
        else if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                writeCallback(0, INTERRUPT);
                return 0;
            }
            else {
                if ((int) d_writeBuffer.size() < d_writeBufferOffset +
                                                                      length) {
                    d_writeBuffer.resize(d_writeBufferOffset + length);
                }
                int len = btes_IovecUtil::gather(
                    &d_writeBuffer.front() + d_writeBufferOffset,
                    d_writeBuffer.size() - d_writeBufferOffset,
                    buffers, numBuffers);
                BSLS_ASSERT(length == len);

                d_writeBufferOffset += length;

                btesos_TcpCbChannel_WReg *request =
                    new (d_wrequestPool)
                        btesos_TcpCbChannel_WReg(length, length,
                                                         writeCallback,
                                                         flags);
                d_writeRequests.push_front(request);
            }
        }
        else {
            writeCallback(s, 0);
            return 0;
        }
        if (0 != d_wManager_p->registerSocketEvent(d_socket_p->handle(),
                                          bteso_EventType::BTESO_WRITE,
                                          d_bufferedWriteFunctor)) {
            btesos_TcpCbChannel_WReg *request = d_writeRequests.back();
            request->invoke(0, CANCELLED);
            d_writeRequests.pop_back();
            d_wrequestPool.deleteObjectRaw(request);
            return -1;
        }
    }
    else {
        int length = btes_IovecUtil::length(buffers, numBuffers);
        if ((int) d_writeBuffer.size() < d_writeBufferOffset + length) {
            d_writeBuffer.resize(d_writeBufferOffset + length);
        }
        int len = btes_IovecUtil::gather(
            &d_writeBuffer.front() + d_writeBufferOffset,
            d_writeBuffer.size() - d_writeBufferOffset,
            buffers, numBuffers);
        BSLS_ASSERT(length == len);

        d_writeBufferOffset += length;

        btesos_TcpCbChannel_WReg *request =
            new (d_wrequestPool) btesos_TcpCbChannel_WReg(length,
                                                 length,
                                                 writeCallback, flags);

        d_writeRequests.push_front(request);
    }
    return 0;
}

///Cancel section
///--------------

void btesos_TcpCbChannel::cancelAll() {
    cancelRead();
    cancelWrite();
}

void btesos_TcpCbChannel::cancelRead() {
    if (d_currentReadRequest_p) {
        // A callback is active -- can't destroy current request.

        bsl::deque<btesos_TcpCbChannel_RReg *> toBeCancelled(
                           d_readRequests.begin(),
                           d_readRequests.begin() + d_readRequests.size() - 1,
                           d_allocator_p);
        d_readRequests.erase(d_readRequests.begin(),
                           d_readRequests.begin() + d_readRequests.size() - 1);
        dequeue(&toBeCancelled, 0, DEQUEUED, &d_rrequestPool);
        BSLS_ASSERT(d_currentReadRequest_p == d_readRequests.back());
    }
    else {
        bsl::deque<btesos_TcpCbChannel_RReg *>
                                  toBeCancelled(d_readRequests, d_allocator_p);
        d_readRequests.clear();
        int numToCancel = toBeCancelled.size();

        if (numToCancel) {
            d_rManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                                bteso_EventType::BTESO_READ);
        }
        dequeue(&toBeCancelled, 0, DEQUEUED, &d_rrequestPool);
    }
}

void btesos_TcpCbChannel::cancelWrite() {
    if (d_currentWriteRequest_p) {
        // A callback is active -- can't destroy current request.

        bsl::deque<btesos_TcpCbChannel_WReg *> toBeCancelled(
                          d_writeRequests.begin(),
                          d_writeRequests.begin() + d_writeRequests.size() - 1,
                          d_allocator_p);

        d_writeRequests.erase(d_writeRequests.begin(),
                              d_writeRequests.begin()
                                                 + d_writeRequests.size() - 1);
        dequeue(&toBeCancelled, 0, DEQUEUED, &d_wrequestPool);
        BSLS_ASSERT(d_currentWriteRequest_p == d_writeRequests.back());
    }
    else {
        bsl::deque<btesos_TcpCbChannel_WReg *>
                                 toBeCancelled(d_writeRequests, d_allocator_p);
        d_writeRequests.clear();
        int numToCancel = toBeCancelled.size();
        if (numToCancel) {
            d_wManager_p->deregisterSocketEvent(d_socket_p->handle(),
                                               bteso_EventType::BTESO_WRITE);
        }
        dequeue(&toBeCancelled, 0, DEQUEUED, &d_wrequestPool);
    }
}

void btesos_TcpCbChannel::invalidate() {
    d_isInvalidReadFlag = 1;
    d_isInvalidWriteFlag = 1;
}

void btesos_TcpCbChannel::invalidateRead() {
    d_isInvalidReadFlag = 1;
}

void btesos_TcpCbChannel::invalidateWrite() {
    d_isInvalidWriteFlag = 1;
}

void btesos_TcpCbChannel::setReadEventManager(
        bteso_TimerEventManager *manager) {
    BSLS_ASSERT(manager);
    BSLS_ASSERT(0 == numPendingReadOperations());
    d_rManager_p = manager;
}

void btesos_TcpCbChannel::setWriteEventManager(
        bteso_TimerEventManager *manager) {
    BSLS_ASSERT(manager);
    BSLS_ASSERT(0 == numPendingWriteOperations());
    d_wManager_p = manager;
}

// ACCESSORS

int btesos_TcpCbChannel::isInvalidRead() const {
   return d_isInvalidReadFlag ;
}

int btesos_TcpCbChannel::isInvalidWrite() const {
   return d_isInvalidWriteFlag ;
}

int btesos_TcpCbChannel::numPendingReadOperations() const {
    return d_readRequests.size();
}

int btesos_TcpCbChannel::numPendingWriteOperations() const {
    return d_writeRequests.size();
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
