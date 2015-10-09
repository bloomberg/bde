// btlmt_channelpool.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_channelpool.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <btls_iovecutil.h>
#include <btlso_resolveutil.h>
#include <btlso_socketimputil.h>
#include <btlso_lingeroptions.h>
#include <btlso_socketoptions.h>
#include <btlso_socketoptutil.h>

#include <bdlma_concurrentpool.h>
#include <btlb_blob.h>
#include <btlb_pooledblobbufferfactory.h>
#include <bdlma_deleter.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <btlb_blobstreambuf.h>

#include <bdlt_currenttime.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bdlb_nullablevalue.h>

#include <bslma_default.h>
#include <bslmf_metaint.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef min
#undef min
#endif

namespace BloombergLP {

namespace btlmt {

typedef btlso::StreamSocket<btlso::IPv4Address>        StreamSocket;
typedef btlso::StreamSocketFactory<btlso::IPv4Address> StreamSocketFactory;

typedef btlso::StreamSocketFactoryAutoDeallocateGuard<btlso::IPv4Address>
                                                       AutoCloseSocket;

using namespace bdlf::PlaceHolders;

enum {

    k_MAX_IOVEC_SIZE =  btlmt::ChannelPool_MessageUtil::e_MAX_IOVEC_SIZE
};

// IMPLEMENTATION NOTES: This implementation file is quite large and uses a
// number of auxiliary classes.  We start with the auxiliary class definitions,
// then with their implementation, then finally provide the class
// implementation of 'btlmt::ChannelPool'.

// NOTE: There is a race in the channel destruction.  Previously, the race was
// resolved by ensuring that the channel destructor was always invoked in the
// same thread (the dispatcher thread of the channel's event manager).  This
// was ensured by a channel wrapper.  In this version, all the callbacks to a
// channel take an additional argument of type 'ChannelHandle', which is
// originally stored in the 'd_channels' catalog.  In this way, the channel
// lives at least as long as its last reference (either in the 'd_channels'
// catalog in the channel pool, or in the time queue of the event manager since
// the shared pointer is stored as extra argument to the callback, or during
// the callback since the shared pointer lives during the entire call).  This
// is perfectly safe and simplifies the logic by removing the need for a
// channel wrapper, since without connectors the channel destructor need no
// longer be executed in the dispatcher thread.  The callbacks that can be put
// into the event manager timer queue (via execute, with a shared handle to the
// channel) are:
//..
//  btlmt::Channel::readTimeoutCb              // via registerTimer
//  btlmt::Channel::registerWriteCb            // via execute
//  btlmt::Channel::disableRead                // via execute
//  btlmt::Channel::initiateReadSequence       // via execute
//  btlmt::Channel::invokeChannelDown          // via execute
//  btlmt::Channel::invokeChannelUp            // via execute
//..
// and the callbacks that can be registered to a socket event (with a shared
// handle to the channel) are:
//..
//  btlmt::Channel::readCb                     // registered to READ using
//                                             // a 'btlb::Blob'
//  btlmt::Channel::writeCb                    // registered to WRITE
//..
// Finally note that those functions *must* take the shared pointer by value
// (not by 'const' reference) or else a 'deregisterTimer' could invalidate the
// shared pointer while the callback is being executed!

// NOTE: There is another race in server shutdown, similar to the channel
// destruction race.  Namely, since 'close' can be called from any thread, even
// though it has a lock on the acceptors map, it should not destroy the
// 'btlmt::ServerState', because there may be other callbacks enqueue that have
// a reference to that server state.  We use the same technique with a shared
// pointer to the server state to ensure that destruction occurs *only* after
// the last scheduled callback has run.  As before, we make the callbacks take
// the shared pointer *by* *value*.

// ============================================================================
//                     LOCAL CLASS DEFINITIONS
// ============================================================================

enum {
    // Metrics categories.

    e_IO_BOUND           = 0,
    e_CPU_BOUND          = 1,

    // Spin optimization for enableRead waiting for e_CHANNEL_UP.

    k_MAX_SPIN           = 1000,         // iterations

    // Exponential backoff parameters in acceptCb (if FD limit reached).

    k_WAIT_FOR_RESOURCES = 1,            // 1s
    k_MAX_EXP_BACKOFF    = 64,           // 64s

    // Error codes

    e_SET_NONBLOCKING_FAILED = -7, // matches what 'listen' returns
    e_FAILED_RESOLUTION      = -2,
    e_NOT_RUNNING            = -1,
    e_SUCCESS                =  0,
    e_DUPLICATE_ID           =  1
};

enum ChannelDownMask {
    // Channel down masks (for half-closed connections).

    e_CLOSED_SEND_MASK    = 0x1,
    e_CLOSED_RECEIVE_MASK = 0x2,
    e_CLOSED_BOTH_MASK    = e_CLOSED_SEND_MASK | e_CLOSED_RECEIVE_MASK
};

                    // ===================
                    // local class Channel
                    // ===================

class Channel {
    // This channel class provides roughly an implementation of the
    // 'btlmt_asyncchannel' protocol for use in this channel pool.  It owns its
    // underlying socket and is responsible for destroying it upon destruction
    // of this channel using a factory passed at construction.
    //
    // Reading from a channel is triggered by a registered socket event which
    // calls, when data is available for reading, calling 'readCb'.  This
    // method extracts the data from the socket, appends it into a 'btlb::Blob'
    // and invokes the registered data callback as needed.
    //
    // Writing is done first in the calling thread, and if no more space is
    // available on the socket, is enqueued into the outgoing message, or if
    // writing of an outgoing message is already in progress, enqueues data
    // into the outgoing blob.  The 'writeCb' is triggered by a registered
    // socket event when space becomes available in the socket outgoing
    // buffers.  It pull the buffers from the outgoing message.  The outgoing
    // blob is transferred to the outgoing message when the outgoing message is
    // empty.  As an optimization, when both the outgoing message and the
    // outgoing blob are empty, the write methods attempt to write right away
    // and if that does not succeed, enqueue in the outgoing message and
    // schedule a write functor into the manager.  In this way, all writes take
    // place from the manager's dispatcher thread, except if writing directly
    // from the calling thread when the outgoing queue is empty.
    // Synchronization between these two modes is done using the outgoing flag,
    // while synchronizing between any outgoing element (outgoing blob,
    // message, or flag) is done using the outgoing mutex.

    // PRIVATE TYPES
    typedef ChannelPool::ChannelStateChangeCallback ChannelStateChangeCallback;
        // Callback called in case of channel events.

    typedef ChannelPool::BlobBasedReadCallback      BlobBasedReadCallback;
        // Blob based callback for reading data, if reading is enabled on this
        // channel.

    typedef bsl::shared_ptr<Channel>                ChannelHandle;
        // Handle shared by all the callbacks and functions which are at some
        // point enqueued in the time queue of an event manager, to ensure that
        // the channel lives at least as long as the callbacks that reference
        // it.

    // PRIVATE DATA MEMBERS

    // Socket section

    bslma::ManagedPtr<StreamSocket>  d_socket;           // socket

    // Callbacks functors section

    ChannelStateChangeCallback       d_channelStateCb;

    BlobBasedReadCallback            d_blobBasedReadCb;

    // Callback data section

    const int                        d_channelId;

    const int                        d_sourceId;

    void                            *d_userData;         // (held)

    // Channel incoming data section

    btls::Iovec                      d_ivecs[k_MAX_IOVEC_SIZE];
                                                         // local buffer,
                                                         // stack-allocated

    int                              d_numUsedIVecs;     // num buffers
                                                         // in 'd_ivecs'

    int                              d_minBytesBeforeNextCb;
                                                         // minimum number of
                                                         // bytes to be
                                                         // accumulated before
                                                         // invoking next user
                                                         // callback

    // Channel outgoing data section

    btls::Iovec                      d_ovecs[k_MAX_IOVEC_SIZE];
                                                         // local buffer,
                                                         // stack-allocated

    volatile bool                    d_hiWatermarkHitFlag;
                                                         // already full

    // Channel state section (here for packing boolean flags together)

    char                             d_channelType;      // created how?

    bool                             d_enableReadFlag;   // are we reading?

    // Channel parameters section

    bool                             d_keepHalfOpenMode;

    const bool                       d_useReadTimeout;   // 'true' if read
                                                         // timeouts should be
                                                         // registered

    bsls::TimeInterval               d_readTimeout;      // read timeout
                                                         // interval

    int                              d_writeCacheLowWat;

    int                              d_writeCacheHiWat;

    const int                        d_minIncomingMessageSize;

    // Channel state section (continued)

    bsls::AtomicInt                  d_channelDownFlag;  // are we down?

    volatile int                     d_channelUpFlag;    // are we running?

    // Channel managers section
    ChannelPool                     *d_channelPool_p;    // (held)

    TcpTimerEventManager            *d_eventManager_p;   // (held)

    void                            *d_readTimeoutTimerId;

    // Channel statistics section
    bsls::TimeInterval               d_creationTime;     // time this object
                                                         // was created

    volatile bsls::Types::Int64      d_numBytesRead;     // bytes read from
                                                         // channel, should
                                                         // only be updated by
                                                         // callbacks
                                                         // executing in the
                                                         // event manager
                                                         // thread

    volatile bsls::Types::Int64      d_numBytesWritten;  // bytes written to
                                                         // channel,
                                                         // modification
                                                         // synchronized with
                                                         // output to channel
                                                         // (i.e.,
                                                         // 'd_writeMutex' and
                                                         // 'd_isWriteActive')

    volatile bsls::Types::Int64      d_numBytesRequestedToBeWritten;
                                                         // bytes requested to
                                                         // be written to
                                                         // channel;
                                                         // modification
                                                         // synchronized with
                                                         // 'd_writeMutex'

    bsls::AtomicInt                  d_recordedMaxWriteCacheSize;
                                                         // maximum recorded
                                                         // size of the write
                                                         // cache

    // DO NOT CHANGE THE ORDER OF THESE TWO DATA MEMBERS

    btlb::BlobBufferFactory         *d_readBlobFactory_p;// factory for
                                                         // 'd_blobReadData'

    btlb::Blob                       d_blobReadData;     // blob for read data

    btlso::IPv4Address               d_peerAddress;      // peer address

    // Memory allocation section (pointers held, not owned)

    btlb::BlobBufferFactory         *d_writeBlobFactory_p;
                                                         // factory for
                                                         // d_writeActiveData
                                                         // and
                                                         // d_writeEnqueuedData

    bsl::shared_ptr<btlb::Blob>      d_writeEnqueuedData;// outgoing msgs,
                                                         // concatenated

    bsl::shared_ptr<btlb::Blob>      d_writeActiveData;  // first msg to go

    int                              d_writeActiveDataCurrentBuffer;

    int                              d_writeActiveDataCurrentOffset;

    bslmt::Mutex                     d_writeMutex;       // synching access to
                                                         // the 'write' data
                                                         // members (variables
                                                         // 'd_write*', and
                                                         // 'd_isWriteActive'.

    bool                             d_isWriteActive;    // a thread is
                                                         // actively writing

    bsls::AtomicInt                  d_writeActiveCacheSize;
                                                         // number of bytes
                                                         // currently being
                                                         // written (including
                                                         // in
                                                         // d_writeActiveData)

    bdlma::ConcurrentPoolAllocator  *d_sharedPtrRepAllocator_p;

    bslma::Allocator                *d_allocator_p;      // for memory

    // PRIVATE MANIPULATORS

    void allocateNextReadBuffers(int numBytes, int totalBufferSize);
        // Allocate the buffers required for the next read operation, where the
        // specified 'numBytes' are read from the socket and there were the
        // specified 'totalBufferSize' bytes provided for the read operation.

    void initDataBufferForReads();
        // Initialize the internal data buffer for the first read operation.

    int populateIVecs();
        // Populate the input vector of buffers passed to 'readv' with the data
        // buffers stored by this channel.  Return the total buffer size
        // available for the 'readv' operation.

    void processReadData(int numBytes);
        // Process the read of the specified 'numBytes' from the socket
        // invoking the user callback if required, and adjusting the internal
        // data buffer as needed.

    // PRIVATE METHODS
    void cancelAll();
        // Remove all the pending timers from the event manager.

    void deregisterSocketRead(ChannelHandle self);
        // Deregister this channel for receiving socket read events.  Must be
        // called only when a read event is registered for the socket
        // underlying this channel in the event manager associated with this
        // channel.

    void deregisterSocketWrite(ChannelHandle self);
        // Deregister this channel for receiving socket write events.  Must be
        // called only when a read event is registered for the socket
        // underlying this channel in the event manager associated with this
        // channel.

    void invokeChannelDown(ChannelHandle              self,
                           ChannelPool::ChannelEvents type);
        // Invoke user-installed channel state callback with the specified
        // 'type' (which can be either 'e_CHANNEL_DOWN',
        // 'e_CHANNEL_DOWN_RECEIVE', or 'e_CHANNEL_DOWN_SEND') in the calling
        // thread.

    void invokeChannelUp(ChannelHandle self);
        // Invoke user-installed channel state callback with 'e_CHANNEL_UP' in
        // the calling thread.  Note that this function should always be
        // executed in the dispatcher thread of the event manager associated
        // with this channel.

    void notifyChannelDown(ChannelHandle             self,
                           btlso::Flag::ShutdownType type,
                           bool                      serializedFlag = true);
        // Shutdown this channel with the specified 'mode' and invoke the
        // user-installed callback for this channel with either
        // 'e_CHANNEL_DOWN', 'e_CHANNEL_DOWN_RECEIVE', or 'e_CHANNEL_DOWN_SEND'
        // depending on the shutdown 'mode', in the event manager's dispatcher
        // thread.  Optionally specify a 'serializedFlag' which, if true,
        // indicates that this function is known to be called from the
        // dispatcher thread *and* that it is OK to invoke the channel state
        // callback directly.  Note that even though 'shutdown()' *can* be
        // called from the dispatcher thread (especially from a callback), it
        // is still *not* OK to invoke the channel state callback directly
        // because this callback may require a resource (e.g., a mutex) which
        // is already acquired by another function higher up in the stack
        // (e.g., the callback that invoked the shutdown in the first place),
        // and this would lead to deadlock.

    int protectAndCheckCallback(
                               const ChannelHandle& self,
                               ChannelDownMask      mask = e_CLOSED_BOTH_MASK);
        // Check that the specified 'self' channel is identical to 'this', and
        // that this method is executed in the dispatcher thread of the event
        // manager associated with this channel.  Return 1 if the channel is
        // down for the specified 'mask' (allowing to check half-open
        // connections), 0 otherwise.

    void readCb(ChannelHandle self);
        // Process data available for reading on this channel.  This callback
        // is registered with read events for the socket underlying this
        // channel, and is called only if data is available or after a timeout
        // (this checks periodically if the peer has disconnected).  Note that
        // this function should always be executed in the dispatcher thread of
        // the event manager associated with this channel.

    void readTimeoutCb(ChannelHandle self);
        // Report that an auto-read operation timed out.  Note that this
        // function should always be executed in the dispatcher thread of the
        // event manager associated with this channel.

    int refillOutgoingMsg();
        // Empty the outgoing message, swap it with the outgoing blob, and
        // return non-zero, if there is more data enqueued in the outgoing
        // blob.  Otherwise, return 0.

    void registerReadTimeoutCallback(bsls::TimeInterval   timeout,
                                     const ChannelHandle& self);
        // Register 'readTimeoutCb' to be called by the manager in its
        // dispatcher thread at the absolute 'timeout'.  Note that this
        // function should always be executed in the dispatcher thread of the
        // event manager associated with this channel.

    void registerWriteCb(ChannelHandle self);
        // Register 'writeCb' to be called by the manager in its dispatcher
        // thread whenever space becomes available in the socket's write
        // buffers.  Note that this function can be invoked from any thread and
        // also that the specified 'self' is guaranteed to live throughout the
        // lifetime of this function call.

    void writeCb(ChannelHandle self);
        // Write the first message(s) enqueued for this channel to the
        // underlying 'StreamSocket'.  If more data is available for writing
        // (either because the write did not completely succeed, or because it
        // did but more data is available in the 'd_writeEnqueuedData'),
        // register a call to 'writeCb' as soon as space is available.  Also
        // ensure that the channel state callback has been called if this
        // channel is down.  This callback is registered with write events for
        // the socket underlying this channel, and is invoked when an outgoing
        // message needs to be written and space is available in the socket's
        // write buffers.  Note that this function should always be executed in
        // the dispatcher thread of the event manager associated with this
        // channel.

    // FRIENDS
    friend class ChannelPool;

  private:
    // NOT IMPLEMENTED
    Channel(const Channel&);
    Channel& operator=(const Channel&);

  public:
    // CREATORS
    Channel(bslma::ManagedPtr<StreamSocket> *socket,
            int                              channelId,
            int                              sourceId,
            const ChannelPoolConfiguration&  configuration,
            ChannelType::Value               channelType,
            bool                             mode,
            ChannelStateChangeCallback       channelCb,
            BlobBasedReadCallback            blobBasedReadCb,
            btlb::BlobBufferFactory         *writeBlobBufferPool,
            btlb::BlobBufferFactory         *readBlobBufferPool,
            TcpTimerEventManager            *eventManager,
            ChannelPool                     *channelPool,
            bdlma::ConcurrentPoolAllocator  *sharedPtrAllocator,
            bslma::Allocator                *basicAllocator = 0);
        // Create a channel belonging to the specified 'channelPool' and
        // managed by the specified 'eventManager' with the specified
        // 'sourceId', 'channelId', and 'configuration'.  Assume ownership of
        // the specified 'socket' to use as the underlying socket.  Load this
        // channel's channel callback with the specified 'channelCb' and
        // 'blobBasedReadCb' to decide which data callback to use.  Use the
        // specified 'sharedPtrAllocator' to create shared pointer rep
        // structures and 'basicAllocator' to supply memory, or the currently
        // installed default allocator if 'basicAllocator' is 0.  Use the
        // specified 'readBlobBufferPool', and 'writeBlobBufferPool' for the
        // blobs used to stored the incoming and outgoing enqueued messages.

    ~Channel();
        // Destroy this channel.

    // MANIPULATORS
    void disableRead(ChannelHandle self, bool enqueueStateChangeCb);
        // Disable automatic reading of data from this channel enqueuing the
        // change state callback if the specified 'enqueueStateChangeCb' is
        // 'true', and invoking it inline otherwise.

    int initiateReadSequence(ChannelHandle self);
        // Schedule an asynchronous timed reading from this channel and also
        // process any incoming data left-over in this channel's internal
        // buffers (otherwise, processing this data would have to wait until
        // more data is enqueued and the read socket event triggered).

    void setUserData(void *userData);
        // Set the opaque user data associated to this channel.

    template <class MessageType>
    int writeMessage(const MessageType&   msg,
                     int                  enqueueWatermark,
                     const ChannelHandle& self);
        // Enqueue the specified 'msg', contained in the templatized type
        // 'MessageType', for writing to this channel, and protect the channel
        // from being destroyed (by another thread) during the course of this
        // operation by holding the specified 'self' handle to this channel
        // pool.  Return 0 on success, or a negative value if the message could
        // not be enqueued.  The templatized type 'MessageType' must be support
        // by the 'ChannelPool_MessageUtil' class (i.e.  'btlb::Blob' or
        // 'ChannelPool_MessageUtil::IovecArray').  Note that the message may
        // not be enqueued if the number of bytes already enqueued for this
        // channel exceeds either the specified 'enqueueWaterMark' or the high
        // water mark specified to this channel at construction, or if
        // enqueuing this message would cause the high water mark to be
        // exceeded.  Note that success does not imply that the message is
        // actually written to the channel, only that it has been enqueued
        // successfully.  Also note that 'self' is guaranteed to be valid
        // during the entirety of this call.

    int setWriteCacheHiWatermark(int numBytes);
        // Set the write cache high-water mark for this channel to the
        // specified 'numBytes'; return 0 on success, and a non-zero value if
        // 'numBytes' is less than the low-water mark for the write cache.  The
        // behavior is undefined unless '0 <= numBytes'.

    void setWriteCacheHiWatermarkRaw(int numBytes);
        // Set the write cache high-water mark for this channel to the
        // specified 'numBytes'.  The behavior is undefined unless exclusive
        // write access has been obtained on this channel prior to the call.

    int setWriteCacheLowWatermark(int numBytes);
        // Set the write cache low-water mark for this channel to the specified
        // 'numBytes'; return 0 on success, and a non-zero value if 'numBytes'
        // is greater than the high-water mark for the write cache.  The
        // behavior is undefined unless '0 <= numBytes'.

    void setWriteCacheLowWatermarkRaw(int numBytes);
        // Set the write cache low-water mark for this channel to the specified
        // 'numBytes'.  The behavior is undefined unless exclusive write access
        // has been obtained on this channel prior to the call.

    void setWriteCacheWatermarks(int lowWatermark, int hiWatermark);
        // Set the write cache low-water and high-water marks for this channel
        // to the specified 'lowWatermark' and 'hiWatermark', respectively.
        // The behavior is undefined unless '0 <= lowWatermark' and
        // 'lowWatermark <= hiWatermark'.

    void resetRecordedMaxWriteCacheSize();
        // Reset the recorded max write cache size for this channel to the
        // current write cache size.  Note that this function resets the
        // recorded max write cache size and does not change the write cache
        // high-water mark for this channel.

    // ACCESSORS
    int channelId() const;
        // Return the unique identifier of this channel.

    bsls::TimeInterval creationTime() const;
        // Return the local time recorded at construction of this channel.

    ChannelType::Value channelType() const;
        // Return an enumerated value identifying how this channel was created
        // from within the channel pool.

    TcpTimerEventManager *eventManager() const;
        // Return a pointer to this channel's event manager.

    const btlso::IPv4Address& peerAddress() const;
        // Return the address of the peer that this channel is connected to.

    bool isChannelDown(ChannelDownMask mask) const;
        // Return 'true' is this channel is down for the specified 'mask'
        // (meaning completely down if 'mask' is e_CLOSED_BOTH_MASK, and
        // completely down or down at least for the corresponding half-duplex
        // connection if 'mask' is 'e_CLOSED_RECEIVE_MASK' or
        // 'e_CLOSED_SEND_MASK'), and 'false' otherwise.

    bsls::Types::Int64 numBytesRead() const;
        // Return the number of bytes read from this channel since its
        // construction or since the last reset.

    bsls::Types::Int64 numBytesWritten() const;
        // Return the number of bytes written to this channel since its
        // construction or since the last reset.

    bsls::Types::Int64 numBytesRequestedToBeWritten() const;
        // Return the number of bytes request to be written to this channel
        // since its construction or since the last reset.

    int currentWriteCacheSize() const;
        // Return a snapshot of the number of bytes currently cached to be
        // written to this channel.

    int recordedMaxWriteCacheSize() const;
        // Return a snapshot of the maximum recorded size, in bytes, of the
        // cache of data to be written to this channel.

    StreamSocket *socket() const;
        // Return a pointer to this channel's underlying socket.

    int sourceId() const;
        // Return the source identifier of this channel.

    void *userData() const;
        // Return the opaque user data associated to this channel.
};

// PRIVATE MANIPULATORS
inline
void Channel::deregisterSocketRead(ChannelHandle)
{
    d_eventManager_p->deregisterSocketEvent(this->socket()->handle(),
                                            btlso::EventType::e_READ);
}

inline
void Channel::deregisterSocketWrite(ChannelHandle)
{
    if (d_eventManager_p->isRegistered(socket()->handle(),
                                       btlso::EventType::e_WRITE)) {
        d_eventManager_p->deregisterSocketEvent(socket()->handle(),
                                                btlso::EventType::e_WRITE);
    }
}

// MANIPULATORS
inline
void Channel::setUserData(void *userData)
{
    d_userData = userData;
}

// ACCESSORS
inline
int Channel::channelId() const
{
    return d_channelId;
}

inline
bsls::TimeInterval Channel::creationTime() const
{
    return d_creationTime;
}

inline
ChannelType::Value Channel::channelType() const
{
    return static_cast<ChannelType::Value>(d_channelType);
}

inline
TcpTimerEventManager *Channel::eventManager() const
{
    return d_eventManager_p;
}

inline
bool Channel::isChannelDown(ChannelDownMask mask) const
{
    return mask == (d_channelDownFlag.loadRelaxed() & mask);
}

inline
const btlso::IPv4Address& Channel::peerAddress() const
{
    return d_peerAddress;
}

inline
bsls::Types::Int64 Channel::numBytesRead() const
{
    return d_numBytesRead;
}

inline
bsls::Types::Int64 Channel::numBytesWritten() const
{
    return d_numBytesWritten;
}

inline
bsls::Types::Int64 Channel::numBytesRequestedToBeWritten() const
{
    return d_numBytesRequestedToBeWritten;
}

inline
int Channel::currentWriteCacheSize() const
{
    return d_writeEnqueuedData->length() +
                                          d_writeActiveCacheSize.loadRelaxed();
}

inline
int Channel::recordedMaxWriteCacheSize() const
{
    return d_recordedMaxWriteCacheSize.loadRelaxed();
}

inline
StreamSocket *Channel::socket() const
{
    return d_socket.ptr();
}

inline
int Channel::sourceId() const
{
    return d_sourceId;
}

inline
void *Channel::userData() const
{
    return d_userData;
}

                    // =====================
                    // local class Connector
                    // =====================

class Connector {
  public:
    // This small object is stored in channel pool 'd_connectors' and holds all
    // (but does not own any of) the information needed while an asynchronous
    // call to 'ChannelPool::connect' is in progress.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Connector,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // DATA MEMBERS
    bsl::shared_ptr<StreamSocket>  d_socket;           // connecting socket

    TcpTimerEventManager          *d_manager_p;        // event manager in
                                                       // which the timeout is
                                                       // registered (not
                                                       // necessarily the one
                                                       // used for creating the
                                                       // connection channel)

    bsl::string                    d_serverName;       // server name to
                                                       // resolve, unused
                                                       // unless the resolution
                                                       // flag is set

    btlso::IPv4Address             d_serverAddress;    // server to connect to

    bsls::TimeInterval             d_creationTime;     // time at which
                                                       // connection was
                                                       // initiated

    bsls::TimeInterval             d_period;           // timeout period
                                                       // between connection
                                                       //  attempts

    bsls::TimeInterval             d_start;            // last absolute
                                                       // timeout

    void                          *d_timeoutTimerId;   // timer registered by
                                                       // event manager

    int                            d_numAttempts;      // remaining number of
                                                       // attempts

    bool                           d_inProgress;       // last call to connect
                                                       // returned WOULD_BLOCK,
                                                       // socket event is still
                                                       // registered

    bool                           d_resolutionFlag;   // whether to perform
                                                       // name resolution,
                                                       // inside
                                                       // connectInitiateCb

    bool                           d_readEnabledFlag;  // flag set to initiate
                                                       // read command upon
                                                       // e_CHANNEL_UP

    bool                           d_keepHalfOpenMode; // mode with which
                                                       // connections must
                                                       // create channels

    bdlb::NullableValue<btlso::SocketOptions>
                                   d_socketOptions;    // socket options
                                                       // provided for connect

    bdlb::NullableValue<btlso::IPv4Address>
                                   d_localAddress;     // client address to
                                                       // bind while connecting

    // CREATORS
    Connector(const bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> >&
                                          socket,
              TcpTimerEventManager       *manager,
              int                         numAttempts,
              const bsls::TimeInterval&   interval,
              bool                        readEnabledFlag,
              bool                        keepHalfOpenMode,
              const btlso::SocketOptions *socketOptions = 0,
              const btlso::IPv4Address   *localAddress = 0,
              bslma::Allocator           *basicAllocator = 0);
        // Create an connector initialized with the specified 'manager',
        // 'numAttempts', and 'interval' period parameters and the specified
        // 'readEnabledFlag' and 'keepHalfOpenMode' flags.  If the specified
        // 'socketOptions' and 'localAddress' are not 0, use those parameters.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, use the currently installed default
        // allocator.

    Connector(const Connector& original, bslma::Allocator *basicAllocator = 0);
        // Create a copy of the specified 'original' connector.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, use the currently installed default
        // allocator.
};

// CREATORS
inline
Connector::Connector(
               const bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> >&
                                           socket,
               TcpTimerEventManager       *manager,
               int                         numAttempts,
               const bsls::TimeInterval&   interval,
               bool                        readEnabledFlag,
               bool                        keepHalfOpenMode,
               const btlso::SocketOptions *socketOptions,
               const btlso::IPv4Address   *localAddress,
               bslma::Allocator           *basicAllocator)
: d_socket(socket)
, d_manager_p(manager)
, d_serverName(basicAllocator)
, d_creationTime(bdlt::CurrentTime::now())
, d_period(interval)
, d_start(d_creationTime)
, d_timeoutTimerId(0)
, d_numAttempts(numAttempts)
, d_inProgress(false)
, d_resolutionFlag(false)
, d_readEnabledFlag(readEnabledFlag)
, d_keepHalfOpenMode(keepHalfOpenMode)
{
    BSLS_ASSERT(0 < numAttempts);
    BSLS_ASSERT(0 < interval || 1 == numAttempts);

    if (socketOptions) {
        d_socketOptions = *socketOptions;
    }

    if (localAddress) {
        d_localAddress = *localAddress;
    }
}

Connector::Connector(const Connector&  original,
                     bslma::Allocator *basicAllocator)
: d_socket(original.d_socket)
, d_manager_p (original.d_manager_p)
, d_serverName(original.d_serverName, basicAllocator)
, d_serverAddress(original.d_serverAddress)
, d_creationTime(original.d_creationTime)
, d_period(original.d_period)
, d_start(original.d_start)
, d_timeoutTimerId(original.d_timeoutTimerId)
, d_numAttempts(original.d_numAttempts)
, d_inProgress(original.d_inProgress)
, d_resolutionFlag(original.d_resolutionFlag)
, d_readEnabledFlag(original.d_readEnabledFlag)
, d_keepHalfOpenMode(original.d_keepHalfOpenMode)
, d_socketOptions(original.d_socketOptions)
, d_localAddress(original.d_localAddress)
{
}

                    // =======================
                    // local class ServerState
                    // =======================

class ServerState {
  public:
    // This small object is stored in channel pool 'd_acceptors' and holds all
    // the needed information (but only owns the socket) while the server is
    // open.

    // DATA MEMBERS
    btlso::IPv4Address          d_endpoint;           // server address

    StreamSocket               *d_socket_p;           // accepting socket
                                                      // (owned)

    StreamSocketFactory        *d_factory_p;          // factory used to
                                                      // deallocate this socket
                                                      // (held, not owned)

    TcpTimerEventManager       *d_manager_p;          // event manager in which
                                                      // the accept timeout is
                                                      // registered (not
                                                      // necessarily the one
                                                      // for creating the
                                                      // accepted channel)

    void                       *d_timeoutTimerId;     // timer registered by
                                                      // event manager (held,
                                                      // not owned)

    bsls::TimeInterval          d_creationTime;       // time at which server
                                                      // was created

    bsls::TimeInterval          d_start;              // last absolute timeout

    bsls::TimeInterval          d_timeout;            // timeout period between
                                                      // callbacks

    void                       *d_acceptAgainId;      // exponential backoff
                                                      // registered by event
                                                      // manager (held, not
                                                      // owned)

    int                         d_exponentialBackoff; // after FD limit is
                                                      // reached, to avoid
                                                      // spewing but still keep
                                                      // server open

    bsls::AtomicInt             d_isClosedFlag;       // lets the callback know
                                                      // to exit right away

    bool                        d_isTimedFlag;        // does the server have a
                                                      // timeout period to
                                                      // issue callbacks?

    bool                        d_readEnabledFlag;    // does the server create
                                                      // channels with
                                                      // readEnabledFlag?

    bool                        d_keepHalfOpenMode;   // mode with which
                                                      // server connections
                                                      // must create channels

    // CREATORS
    ~ServerState();
        // Destroy this server,
};

// CREATORS
inline
ServerState::~ServerState()
{
    if (d_socket_p) {
        d_factory_p->deallocate(d_socket_p);
        d_socket_p = 0;  // for debugging now, shouldn't slow anything down
    }
}

// ============================================================================
//                      LOCAL FUNCTIONS IMPLEMENTATIONS
// ============================================================================

void Channel::allocateNextReadBuffers(int numBytes, int totalBufferSize)
{
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= totalBufferSize);

    if (numBytes
     && totalBufferSize == numBytes
     && d_numUsedIVecs < k_MAX_IOVEC_SIZE) {
        ++d_numUsedIVecs;
    }

    d_blobReadData.reserveBufferCapacity(
                             d_blobReadData.numDataBuffers() + d_numUsedIVecs);

    // Note that we must have at least 'd_numUsedIVecs - 1' extra buffers
    // available at the end of 'blob'.

    int       numAvailBuffers = d_blobReadData.numBuffers() -
                                               d_blobReadData.numDataBuffers();
    const int numDataBufs     = d_blobReadData.numDataBuffers();
    const int lastBufferLen   = d_blobReadData.lastDataBufferLength();

    if (numDataBufs
     && lastBufferLen
     && lastBufferLen < d_blobReadData.buffer(numDataBufs - 1).size()) {
        ++numAvailBuffers;
    }

    for (int i = numAvailBuffers; i < d_numUsedIVecs; ++i) {
        btlb::BlobBuffer buffer, newBuffer;
        d_readBlobFactory_p->allocate(&newBuffer);

        buffer.setSize(newBuffer.size());
        d_blobReadData.appendBuffer(buffer);

        const int index = d_blobReadData.numBuffers() - 1;
        d_blobReadData.swapBufferRaw(index, &newBuffer);
    }
}

void Channel::initDataBufferForReads()
{
    // Initializes incoming message: allocate just one buffer, and sets the
    // capacity (field 1).

    btlb::BlobBuffer buffer, newBuffer;
    d_readBlobFactory_p->allocate(&newBuffer);

    buffer.setSize(newBuffer.size());
    d_blobReadData.appendBuffer(buffer);

    const int index = d_blobReadData.numBuffers() - 1;
    d_blobReadData.swapBufferRaw(index, &newBuffer);
    d_minBytesBeforeNextCb = d_minIncomingMessageSize;
}

int Channel::populateIVecs()
{
    BSLS_ASSERT(d_numUsedIVecs <=
            d_blobReadData.numBuffers() - d_blobReadData.numDataBuffers() + 1);

    int          totalBufferSize = 0;
    int          startIdx        = d_blobReadData.numDataBuffers();
    int          lastBufferLen   = d_blobReadData.lastDataBufferLength();
    int          numIVecsToFill  = d_numUsedIVecs;
    btls::Iovec *ivec            = d_ivecs;

    if (lastBufferLen
     && lastBufferLen < d_blobReadData.buffer(startIdx - 1).size()) {
        const btlb::BlobBuffer& buffer = d_blobReadData.buffer(startIdx - 1);
        const int remainingBufferLen = buffer.size() - lastBufferLen;

        bsls::PerformanceHint::prefetchForWriting(buffer.data() +
                                                                lastBufferLen);
        ivec->setBuffer(static_cast<void *>(buffer.data() + lastBufferLen),
                        remainingBufferLen);
        totalBufferSize += remainingBufferLen;
        ++ivec;
        --numIVecsToFill;
    }

    for (int i = 0; i < numIVecsToFill; ++i, ++ivec) {
        const btlb::BlobBuffer& buffer = d_blobReadData.buffer(startIdx + i);

        bsls::PerformanceHint::prefetchForWriting(buffer.data());
        ivec->setBuffer(static_cast<void *>(buffer.data()), buffer.size());
        totalBufferSize += buffer.size();
    }
    return totalBufferSize;
}

void Channel::processReadData(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    int blobDataLen = d_blobReadData.length() + numBytes;
    d_blobReadData.setLength(blobDataLen);
    if (blobDataLen >= d_minBytesBeforeNextCb) {
        int minAdditional = -1;

        // Invariant (must be verified prior to callback): this can
        // shed additional unused buffers in 'blob', but they are
        // regrown inside the calling 'readCb'.

        d_blobBasedReadCb(&minAdditional,
                          &d_blobReadData,
                          d_channelId,
                          d_userData);
        BSLS_ASSERT(0 < minAdditional);

        d_minBytesBeforeNextCb = minAdditional;
    }
}
}  // close package namespace

// ============================================================================
//                        LOCAL CLASS IMPLEMENTATIONS
// ============================================================================

typedef bsl::shared_ptr<btlmt::ServerState> ServerHandle;
typedef bsl::map<int, ServerHandle>         ServerStateMap;
typedef bsl::map<int, btlmt::Connector>     ConnectorMap;
typedef bsl::map<int, btlmt::TimerState>    TimerStateMap;

namespace btlmt {
                    // -------------------
                    // local class Channel
                    // -------------------

// PRIVATE MANIPULATORS
void Channel::invokeChannelDown(ChannelHandle              self,
                                ChannelPool::ChannelEvents type)
{
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                                  bslmt::ThreadUtil::self(),
                                  d_eventManager_p->dispatcherThreadHandle()));
    BSLS_ASSERT(this == self.get());

    // We are guaranteed that 'self' is valid, and holding this 'self'
    // channel will delay the destruction of this channel until at least the
    // callback completes.

    if (ChannelPool::e_CHANNEL_DOWN_READ == type) {
        if (d_eventManager_p->isRegistered(socket()->handle(),
                                           btlso::EventType::e_READ)) {
            d_eventManager_p->deregisterSocketEvent(socket()->handle(),
                                                    btlso::EventType::e_READ);
        }
    }
    else if (ChannelPool::e_CHANNEL_DOWN == type) {
        d_eventManager_p->deregisterSocket(socket()->handle());
    }

    // Do not deregister the read time out if not closing the read part.

    if (ChannelPool::e_CHANNEL_DOWN_WRITE != type) {
        if (d_readTimeoutTimerId) {
            d_eventManager_p->deregisterTimer(d_readTimeoutTimerId);
            d_readTimeoutTimerId = 0;
        }
    }

    d_channelStateCb(d_channelId, d_sourceId, type, d_userData);
    d_channelUpFlag = 0;
}

void Channel::invokeChannelUp(ChannelHandle)
{
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                                  bslmt::ThreadUtil::self(),
                                  d_eventManager_p->dispatcherThreadHandle()));

    d_channelStateCb(d_channelId,
                     d_sourceId,
                     ChannelPool::e_CHANNEL_UP,
                     d_userData);

    d_channelUpFlag = 1;
}

void Channel::notifyChannelDown(ChannelHandle             self,
                                btlso::Flag::ShutdownType type,
                                bool                      serializedFlag)
{
    // Always executed in the event manager's dispatcher thread, *except* if
    // called from ChannelPool::shutdown (for e_IMMEDIATE), or from
    // registerWriteCb.

    // Note: It is *not* OK to compute serializedFlag as follows:
    // bool serializedFlag = bslmt::ThreadUtil::isEqual(
    //                             bslmt::ThreadUtil::self(),
    //                             d_eventManager_p->dispatcherThreadHandle());
    // See the "Note that" in this function-level documentation as to why.

    if (!d_keepHalfOpenMode) {
        // No support for half-open connections, simply shut down, irrespective
        // of 'type'.

        type = btlso::Flag::e_SHUTDOWN_BOTH;
    }

    // Determine mask to apply to channel down flag.

    int channelDownMask = e_CLOSED_BOTH_MASK;

    if (btlso::Flag::e_SHUTDOWN_RECEIVE == type) {
        channelDownMask = e_CLOSED_RECEIVE_MASK;
    }

    if (btlso::Flag::e_SHUTDOWN_SEND == type) {
        channelDownMask = e_CLOSED_SEND_MASK;
    }

    // Atomically apply the mask to d_channelDownFlag and determine which
    // callbacks we are responsible for.

    int channelDownFlag    = d_channelDownFlag;
    int newChannelDownFlag = channelDownFlag | channelDownMask;

    while (channelDownFlag != d_channelDownFlag.testAndSwap(
                                                         channelDownFlag,
                                                         newChannelDownFlag)) {
        channelDownFlag    = d_channelDownFlag;
        newChannelDownFlag = channelDownFlag | channelDownMask;
    }

    // Invoke appropriate callback(s).  Note that it is possible to invoke one,
    // two, or all three callbacks in half-open mode, but only the last one if
    // half-open mode is not enabled.

    if (d_keepHalfOpenMode
     && (channelDownFlag & e_CLOSED_RECEIVE_MASK)
                             != (newChannelDownFlag & e_CLOSED_RECEIVE_MASK)) {
        if (serializedFlag) {
            invokeChannelDown(self, ChannelPool::e_CHANNEL_DOWN_READ);
        }
        else {
            bsl::function<void()> cb(bdlf::BindUtil::bind(
                                           &Channel::invokeChannelDown,
                                            this,
                                            self,
                                            ChannelPool::e_CHANNEL_DOWN_READ));
            d_eventManager_p->execute(cb);
        }
    }

    if (d_keepHalfOpenMode
     && (channelDownFlag & e_CLOSED_SEND_MASK)
                                != (newChannelDownFlag & e_CLOSED_SEND_MASK)) {
        if (serializedFlag) {
            invokeChannelDown(self, ChannelPool::e_CHANNEL_DOWN_WRITE);
        }
        else {
            bsl::function<void()> cb(bdlf::BindUtil::bind(
                                          &Channel::invokeChannelDown,
                                           this,
                                           self,
                                           ChannelPool::e_CHANNEL_DOWN_WRITE));
            d_eventManager_p->execute(cb);
        }
    }

    if (channelDownFlag    != newChannelDownFlag
     && newChannelDownFlag == e_CLOSED_BOTH_MASK) {
        bsls::TimeInterval intv =
                             (d_channelPool_p->d_lastResetTime > d_creationTime
                             ? d_channelPool_p->d_lastResetTime
                             : d_creationTime);

        intv = bdlt::CurrentTime::now() - intv;

        d_channelPool_p->d_totalConnectionsLifetime.add(
                                   static_cast<int>(intv.totalMicroseconds()));

        {
            // Note that this lock must be held to ensure that updating the
            // adjustment to the metric totals, and removing the channel is
            // handled atomically.

            bslmt::LockGuard<bslmt::Mutex> guard(
                                    &d_channelPool_p->d_metricAdjustmentMutex);

            d_channelPool_p->d_totalBytesWrittenAdjustment +=
                                                             d_numBytesWritten;
            d_channelPool_p->d_totalBytesReadAdjustment    += d_numBytesRead;
            d_channelPool_p->d_totalBytesRequestedWrittenAdjustment +=
                                               d_numBytesRequestedToBeWritten;

            int rc = d_channelPool_p->d_channels.remove(d_channelId);

            BSLS_ASSERT(0 == rc);
        }

        if (serializedFlag) {
            invokeChannelDown(self, ChannelPool::e_CHANNEL_DOWN);
        }
        else {
            bsl::function<void()> cb(bdlf::BindUtil::bind(
                                                &Channel::invokeChannelDown,
                                                 this,
                                                 self,
                                                 ChannelPool::e_CHANNEL_DOWN));

            d_eventManager_p->execute(cb);
        }
    }
}

inline
int Channel::protectAndCheckCallback(const ChannelHandle& self,
                                     ChannelDownMask      mask)
{
    BSLS_ASSERT(this == self.get());
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                                  bslmt::ThreadUtil::self(),
                                  d_eventManager_p->dispatcherThreadHandle()));

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(isChannelDown(mask))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 1;                                                     // RETURN
    }

    // Else we are guaranteed that this channel is valid, and since all callers
    // hold the shared pointer 'self' to this channel, will delay the
    // destruction of this channel until at least the caller completes.

    return 0;
}

void Channel::readCb(ChannelHandle self)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                  0 != protectAndCheckCallback(self, e_CLOSED_RECEIVE_MASK))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_enableReadFlag)) {
        // This readCb was still pending while we were executing 'disableRead'
        // and didn't get properly deregistered.  We abort now to avoid
        // re-registering a readTimeout callback in the loop below.

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == d_numUsedIVecs)) {
        // Create incoming message.

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        initDataBufferForReads();
        d_numUsedIVecs = 1;
    }

    int skip = 0;
    while (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!d_channelUpFlag)) {
        // Spin until e_CHANNEL_UP messages is delivered.  This is done so that
        // the user won't get DATA callback before a e_CHANNEL_UP] event is
        // delivered.  We don't expect the loop to take very long, but just in
        // case let's yield if the flag doesn't catch up.

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        if (skip < k_MAX_SPIN) {
            ++skip;
        }
        else {
            bslmt::ThreadUtil::yield();
        }
    }
    bsls::PerformanceHint::prefetchForReading(d_userData);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                       isChannelDown(e_CLOSED_RECEIVE_MASK))) {
        // This readCb was invoked by a socket event after we executed
        // 'notifyChannelDown' in a different thread (from 'shutdown') and the
        // socket has not yet been deregistered (in pending
        // 'invokeChannelDown').  We abort.  The reason the test is performed
        // so late in 'readCb' is to allow picking up such events as late as
        // possible.  Note that this test is done again in the loop below.

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    // Note that 'lastRead' is used only to re-initialize the read timeout.

    bsls::TimeInterval lastRead;
    if (d_useReadTimeout) {
        lastRead = bdlt::CurrentTime::now();
    }

    int readRet = 0;
    while (1) {
        // Because buffered sockets (e.g. OpenSsl) may hold data internally,
        // they may not trigger a call to readCb, so we must attempt to read as
        // much as possible and always read more while data is still available.
        // This is not optimal from the point of view of load balancing, it
        // would be better if the manager had the option of calling back this
        // readCb, but until the socket has a way of being queried, there is no
        // way to know and so we must be greedy in reading or risk never being
        // called back again.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                       isChannelDown(e_CLOSED_RECEIVE_MASK))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            return;                                                   // RETURN
        }

        int totalBufferSize = populateIVecs();

        readRet = socket()->readv(d_ivecs, d_numUsedIVecs);

        if (readRet < 0) {
            if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == readRet) {
                // No more data to read, we can exit, knowing that 'readCb'
                // will be called again by event manager if data becomes
                // available again.  Reschedule a timeout from the time of the
                // last read and register this socket event.

                break;
            }

            BSLS_ASSERT(btlso::SocketHandle::e_ERROR_INTERRUPTED != readRet);

            notifyChannelDown(self, btlso::Flag::e_SHUTDOWN_RECEIVE);
            return;                                                   // RETURN
        }

        BSLS_ASSERT(0 < readRet);
        BSLS_ASSERT(readRet <= totalBufferSize);

        // Note that 'd_numBytesRead' is only updated by this callback, which
        // executes in the (single) event manager thread.

        d_numBytesRead += readRet;

        if (d_useReadTimeout) {
            lastRead = bdlt::CurrentTime::now();
        }

        processReadData(readRet);
        allocateNextReadBuffers(readRet, totalBufferSize);

        if (!d_enableReadFlag) {
            return;                                                   // RETURN
        }

        if (readRet != totalBufferSize) {
            break;
        }
    }

    if (d_useReadTimeout) {
        const bsls::TimeInterval timeout = lastRead + d_readTimeout;

        if (!d_readTimeoutTimerId) {
            registerReadTimeoutCallback(timeout, self);
        }
        else {
            const int rc = d_eventManager_p->rescheduleTimer(
                                                          d_readTimeoutTimerId,
                                                          timeout);
            BSLS_ASSERT(!rc);
        }
    }
}

void Channel::readTimeoutCb(ChannelHandle self)
{
    if (0 != protectAndCheckCallback(self, e_CLOSED_RECEIVE_MASK)) {
        return;                                                       // RETURN
    }

    d_channelStateCb(d_channelId,
                     d_sourceId,
                     ChannelPool::e_READ_TIMEOUT,
                     d_userData);

    d_readTimeoutTimerId = 0;

    readCb(self);
}

int Channel::refillOutgoingMsg()
{
    BSLS_ASSERT(0 == d_writeActiveData->numBuffers());
    BSLS_ASSERT(0 == d_writeActiveDataCurrentBuffer);
    BSLS_ASSERT(0 == d_writeActiveDataCurrentOffset);

    bslmt::LockGuard<bslmt::Mutex> oGuard(&d_writeMutex);
    BSLS_ASSERT(d_isWriteActive);

    if (0 == d_writeEnqueuedData->length()) {
        // There isn't any pending data.  This must be done while holding the
        // lock because o/w some other thread could come in just after
        // returning and before 'd_isWriteActive = 0' is done by the caller,
        // and enqueue into 'd_writeEnqueuedData' instead of
        // 'd_writeActiveData', letting a subsequent call violate the
        // order-preserving property of messages.

        d_isWriteActive = false;
        return 0;                                                     // RETURN
    }

    // Otherwise, swap 'd_writeEnqueuedData' and 'd_writeActiveData'.

    d_writeEnqueuedData.swap(d_writeActiveData);
    d_writeActiveCacheSize.addRelaxed(d_writeActiveData->length());

    return 1;
}

void Channel::registerReadTimeoutCallback(bsls::TimeInterval   timeout,
                                          const ChannelHandle& self)
{
    // Always called from 'readCb' or from 'initiateReadSequence', no need to
    // 'protectAndCheckCallback'.

    BSLS_ASSERT(0 == d_readTimeoutTimerId);

    bsl::function<void()> readTimeoutFunctor(bdlf::BindUtil::bind(
                                                       &Channel::readTimeoutCb,
                                                        this,
                                                        self));

    d_readTimeoutTimerId = d_eventManager_p->registerTimer(timeout,
                                                           readTimeoutFunctor);
}

void Channel::registerWriteCb(ChannelHandle self)
{
    // This callback is executed whenever data is available in
    // 'd_writeActiveData'.

    if (0 != protectAndCheckCallback(self, e_CLOSED_SEND_MASK)) {
        return;                                                       // RETURN
    }

    bsl::function<void()> writeFunctor(bdlf::BindUtil::bind(&Channel::writeCb,
                                                             this,
                                                             self));

    int rCode = d_eventManager_p->registerSocketEvent(
                                                     this->socket()->handle(),
                                                     btlso::EventType::e_WRITE,
                                                     writeFunctor);

    if (0 != rCode) {
        if (isChannelDown(e_CLOSED_SEND_MASK)) {

            d_channelStateCb(d_channelId,
                             d_sourceId,
                             ChannelPool::e_MESSAGE_DISCARDED,
                             d_userData);
        }
        else {
            notifyChannelDown(self, btlso::Flag::e_SHUTDOWN_SEND);
        }
    }

    // We simply wait until the socket calls us back.
}

void Channel::writeCb(ChannelHandle self)
{
    // This callback is executed whenever the write buffer of 'd_socket_p' has
    // some space available, always in the event manager's dispatcher thread.

    if (0 != protectAndCheckCallback(self)) {
        return;                                                       // RETURN
    }

    // This method is always executed in the dispatcher thread of the event
    // manager, and thus there is no race with 'writeMessage', as long as the
    // outgoing flag is set (since 'writeMessage' will append to the outgoing
    // blob).  Only if we need to get more data from the outgoing blob do we
    // need to acquire the lock (in 'refillOutgoingMsg').  When this method is
    // called, there should always be data available in the outgoing message.

    // BSLS_ASSERT(d_isWriteActive); // not atomic anymore

    // The portion of the current outgoing message not yet written is indicated
    // by the first user data field (current buffer index) and second user data
    // field (offset in current buffer of first byte not written).

    BSLS_ASSERT(0 < d_writeActiveData->length());

    int numBuffers = d_writeActiveData->numDataBuffers();
    BSLS_ASSERT(0 < numBuffers);

    int currentBuffer = d_writeActiveDataCurrentBuffer;
    int currentOffset = d_writeActiveDataCurrentOffset;

    bsls::PerformanceHint::prefetchForReading(
              d_writeActiveData->buffer(currentBuffer).data() + currentOffset);

    BSLS_ASSERT(0 <= currentBuffer);
    BSLS_ASSERT(currentBuffer < numBuffers);

    int bufSize = currentBuffer < numBuffers - 1
                  ? d_writeActiveData->buffer(currentBuffer).size()
                  : d_writeActiveData->lastDataBufferLength();

    BSLS_ASSERT(0 <= currentOffset);
    BSLS_ASSERT(currentOffset < bufSize);

    while (1) {
        // Write data into the socket, using 'writev' for efficiency.

        int numVecs = 1;
        int numMaxVecs = bsl::min(numBuffers - currentBuffer,
                                  static_cast<int>(k_MAX_IOVEC_SIZE));

        d_ovecs[0].setBuffer(
               d_writeActiveData->buffer(currentBuffer).data() + currentOffset,
               bufSize - currentOffset);

        for (int i = currentBuffer + 1; numVecs < numMaxVecs; ++i, ++numVecs) {
            const btlb::BlobBuffer&  blobBuffer = d_writeActiveData->buffer(i);
            char                    *buf = blobBuffer.data();

            bsls::PerformanceHint::prefetchForReading(buf);

            d_ovecs[numVecs].setBuffer(
                                  buf,
                                  i < numBuffers - 1
                                  ? blobBuffer.size()
                                  : d_writeActiveData->lastDataBufferLength());
        }

        int writeRet = socket()->writev(d_ovecs, numVecs);

        if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == writeRet) {
            // In theory, this is the only writing thread so if 'writeCb' we
            // should never get 'EWOULDBLOCK'.  But there is little harm.
            // Let's just wait until this function is called again.  Note that
            // the callback is still registered on WRITE event.

            return;                                                   // RETURN
        }

        // In case of error, shut down channel.  There is no need to call
        // 'deregisterSocketWrite' since 'notifyChannelDown' deregisters all
        // socket events.

        if (writeRet <= 0) {
            BSLS_ASSERT(btlso::SocketHandle::e_ERROR_INTERRUPTED != writeRet);

            if (isChannelDown(e_CLOSED_SEND_MASK)) {
                d_channelStateCb(d_channelId,
                                 d_sourceId,
                                 ChannelPool::e_MESSAGE_DISCARDED,
                                 d_userData);
            }
            else {
                notifyChannelDown(self, btlso::Flag::e_SHUTDOWN_SEND);
            }
            return;                                                   // RETURN
        }

        // Otherwise proceed with reporting.

        { // Lock, just for updating the stats.
            bslmt::LockGuard<bslmt::Mutex> oGuard(&d_writeMutex);

            d_numBytesWritten += writeRet;
            d_writeActiveCacheSize.addRelaxed(-writeRet);

            if (d_hiWatermarkHitFlag
             && (currentWriteCacheSize() <= d_writeCacheLowWat)) {

                d_hiWatermarkHitFlag = false;

                oGuard.release()->unlock();

                d_channelStateCb(d_channelId,
                                 d_sourceId,
                                 ChannelPool::e_WRITE_CACHE_LOWWAT,
                                 d_userData);
            }
        } // End of the lock guard.

        // Advance the buffer count for the outgoing message.  Note that there
        // are no locks required here.  Status gets subtracted by the number of
        // bytes written in the iovec write buffers, and should be 0, except if
        // the last buffer is not completely written.

        while (0 < writeRet && bufSize <= writeRet + currentOffset) {
            writeRet -= bufSize - currentOffset;
            ++currentBuffer;
            currentOffset = 0;
            bufSize = currentBuffer < numBuffers - 1
                      ? d_writeActiveData->buffer(currentBuffer).size()
                      : d_writeActiveData->lastDataBufferLength();
        }
        currentOffset += writeRet;
        BSLS_ASSERT(currentBuffer <= numBuffers);

        // Update the outgoing message with the new current buffer and offset
        // information.

        if (currentBuffer < numBuffers) {
            // The current buffer and offset of the outgoing message may
            // have changed.  This must be recorded.

            d_writeActiveDataCurrentBuffer = currentBuffer;
            d_writeActiveDataCurrentOffset = currentOffset;
        }
        else {
            BSLS_ASSERT(0 == currentOffset);

            // There is no more data to write from 'd_writeActiveData'.  Empty
            // the outgoing message since all the data there has been written.

            d_writeActiveData->removeAll();
            d_writeActiveDataCurrentBuffer = 0;
            d_writeActiveDataCurrentOffset = 0;

            // We must try again with the next messages in the queue, so now we
            // need to lock to gain access to the d_writeEnqueuedData.  This is
            // done in 'refillOutgoingMsg'.

            if (isChannelDown(e_CLOSED_SEND_MASK) || !refillOutgoingMsg()) {
                // There isn't any pending data, our work here is done.

                deregisterSocketWrite(self);
                return;                                               // RETURN
            }

            // Start next write at the beginning of the new blob.  Maintain the
            // invariant that 'bufSize' corresponds to the data length of the
            // current buffer.

            numBuffers    = d_writeActiveData->numDataBuffers();
            currentBuffer = 0;
            currentOffset = 0;
            bufSize       = 1 < numBuffers
                            ? d_writeActiveData->buffer(0).size()
                            : d_writeActiveData->lastDataBufferLength();
        }
    }

    BSLS_ASSERT(0 && "Unreachable by design");
}

// CREATORS
Channel::Channel(bslma::ManagedPtr<StreamSocket> *socket,
                 int                              channelId,
                 int                              sourceId,
                 const ChannelPoolConfiguration&  config,
                 ChannelType::Value               channelType,
                 bool                             mode,
                 ChannelStateChangeCallback       channelCb,
                 BlobBasedReadCallback            blobBasedReadCb,
                 btlb::BlobBufferFactory         *writeBlobBufferPool,
                 btlb::BlobBufferFactory         *readBlobBufferPool,
                 TcpTimerEventManager            *eventManager,
                 ChannelPool                     *channelPool,
                 bdlma::ConcurrentPoolAllocator  *sharedPtrAllocator,
                 bslma::Allocator                *basicAllocator)
: d_socket(*socket)
, d_channelStateCb(channelCb)
, d_blobBasedReadCb(blobBasedReadCb)
, d_channelId(channelId)
, d_sourceId(sourceId)
, d_userData(static_cast<void *>(0))
, d_numUsedIVecs(0)
, d_minBytesBeforeNextCb(config.minIncomingMessageSize())
, d_hiWatermarkHitFlag(false)
, d_channelType(channelType)
, d_enableReadFlag(false)
, d_keepHalfOpenMode(mode)
, d_useReadTimeout(config.readTimeout() > 0.0)
, d_readTimeout(config.readTimeout())
, d_writeCacheLowWat(config.writeCacheLowWatermark())
, d_writeCacheHiWat(config.writeCacheHiWatermark())
, d_minIncomingMessageSize(config.minIncomingMessageSize())
, d_channelDownFlag(0)
, d_channelUpFlag(0)
, d_channelPool_p(channelPool)
, d_eventManager_p(eventManager)
, d_readTimeoutTimerId(0)
, d_creationTime(bdlt::CurrentTime::now())
, d_numBytesRead(0)
, d_numBytesWritten(0)
, d_numBytesRequestedToBeWritten(0)
, d_recordedMaxWriteCacheSize(0)
, d_readBlobFactory_p(readBlobBufferPool)
, d_blobReadData(d_readBlobFactory_p, basicAllocator)
, d_writeBlobFactory_p(writeBlobBufferPool)
, d_writeActiveDataCurrentBuffer(0)
, d_writeActiveDataCurrentOffset(0)
, d_isWriteActive(false)
, d_writeActiveCacheSize(0)
, d_sharedPtrRepAllocator_p(sharedPtrAllocator)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(d_socket);

    d_socket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    d_socket->peerAddress(&d_peerAddress);

#ifdef BSLS_PLATFORM_OS_UNIX
    // Set close-on-exec flag: this only makes sense in Unix, there is no
    // equivalent for Windows.

    int fd    = this->socket()->handle();
    int flags = fcntl(fd, F_GETFD);
    int ret   = fcntl(fd, F_SETFD, flags | FD_CLOEXEC);

    BSLS_ASSERT( -1 != ret);
#endif

    d_writeEnqueuedData.createInplace(d_allocator_p,
                                      d_writeBlobFactory_p,
                                      d_allocator_p);

    d_writeActiveData.createInplace(d_allocator_p,
                                    d_writeBlobFactory_p,
                                    d_allocator_p);
}

Channel::~Channel()
{
    // This destructor can only be called "by" the timer event manager, either
    // through proper scheduling (i.e. channel shutdown in which case the start
    // flag is up), or during the destruction of the channel pool (at which
    // point the start flag is down) when the manager's timer queue is
    // destroyed (because the channel wrappers, destroyed by call to
    // 'd_channels.removeAll()' have enqueued a 'deallocateChannel' functor
    // along with the last shared reference to this channel).  In either case,
    // all calls to callbacks have completed, it is safe to deallocate the
    // socket.  In case the event manager is not being destroyed,
    // 'notifyChannelDown' has already deregistered all socket events
    // pertaining to this deallocated socket.

    BSLS_ASSERT(d_recordedMaxWriteCacheSize >= 0);
}

// MANIPULATORS
void Channel::disableRead(ChannelHandle self, bool enqueueStateChangeCb)
{
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                                  bslmt::ThreadUtil::self(),
                                  d_eventManager_p->dispatcherThreadHandle()));
    BSLS_ASSERT(this == self.get());

    deregisterSocketRead(self);

    if (d_readTimeoutTimerId) {
        d_eventManager_p->deregisterTimer(d_readTimeoutTimerId);
        d_readTimeoutTimerId = 0;
    }
    d_enableReadFlag = false;

    if (enqueueStateChangeCb) {
        bsl::function<void()> stateCbFunctor(
                        bdlf::BindUtil::bind(d_channelStateCb,
                                             d_channelId,
                                             d_sourceId,
                                             ChannelPool::e_AUTO_READ_DISABLED,
                                             d_userData));

        d_eventManager_p->execute(stateCbFunctor);
    }
    else {
        d_channelStateCb(d_channelId,
                         d_sourceId,
                         ChannelPool::e_AUTO_READ_DISABLED,
                         d_userData);
    }
}

int Channel::initiateReadSequence(ChannelHandle self)
{
    if (0 != protectAndCheckCallback(self) || d_enableReadFlag) {
        // There is no point doing this twice, if 'd_enableReadFlag' is already
        // set.

        return 0;                                                     // RETURN
    }
    BSLS_ASSERT(d_channelUpFlag);
    BSLS_ASSERT(d_socket);

    bsl::function<void()> readFunctor = bdlf::BindUtil::bind(&Channel::readCb,
                                                              this,
                                                              self);

    int rCode = d_eventManager_p->registerSocketEvent(this->socket()->handle(),
                                                      btlso::EventType::e_READ,
                                                      readFunctor);

    if (0 == rCode) {
        d_enableReadFlag = true;
        d_channelStateCb(d_channelId,
                         d_sourceId,
                         ChannelPool::e_AUTO_READ_ENABLED,
                         d_userData);

        // Only register a read timeout if there is a valid timeout value.

        if (d_useReadTimeout) {
            registerReadTimeoutCallback(
                               bdlt::CurrentTime::now() + d_readTimeout, self);
        }
    }
    else {
        notifyChannelDown(self, btlso::Flag::e_SHUTDOWN_RECEIVE);
    }

    return rCode;
}

template <class MessageType>
int Channel::writeMessage(const MessageType&   msg,
                          int                  enqueueWatermark,
                          const ChannelHandle& self)
{
    typedef ChannelPool_MessageUtil MessageUtil;

    enum {
        e_CACHE_HIWAT     = -1,
        e_HIT_CACHE_HIWAT = -2,
        e_CHANNEL_DOWN    = -3,
        e_ENQUEUE_WAT     = -4,
        e_SUCCESS         =  0
    };

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                          isChannelDown(e_CLOSED_SEND_MASK))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return e_CHANNEL_DOWN;                                        // RETURN
    }

    bsls::Types::Int64 dataLength = MessageUtil::length(msg);

    // Grab the lock.  Note that we shouldn't release the lock until the
    // message is enqueued for write.  If there are no other messages enqueued,
    // it is enough to set the outgoing flag because that will force any other
    // message to be enqueued to 'd_writeEnqueuedData', while this message can
    // proceed with 'd_writeActiveData' which is disjoint.  Note that in that
    // case, the 'writeCb' method is not running so there is no race condition
    // possible with 'd_writeActiveData'.  If there are other messages
    // enqueued, we need to append at the end of 'd_writeEnqueuedData' and must
    // retain the lock until this is completed to guarantee the integrity of
    // outgoing messages.

    bslmt::LockGuard<bslmt::Mutex> oGuard(&d_writeMutex);

    d_numBytesRequestedToBeWritten += dataLength;

    const int writeCacheSize = currentWriteCacheSize();

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         writeCacheSize > d_writeCacheHiWat)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return e_CACHE_HIWAT;                                         // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                          writeCacheSize > enqueueWatermark)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_hiWatermarkHitFlag = true;
        return e_ENQUEUE_WAT;                                         // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                            writeCacheSize + dataLength > d_writeCacheHiWat)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        if(!d_hiWatermarkHitFlag) {
            d_hiWatermarkHitFlag = true;

            bsl::function<void()> functor(bdlf::BindUtil::bind(
                                              d_channelStateCb,
                                              d_channelId,
                                              d_sourceId,
                                              ChannelPool::e_WRITE_CACHE_HIWAT,
                                              d_userData));

            d_eventManager_p->execute(functor);

            // We must release the mutex AFTER 'functor' is enqueued to be
            // executed.  Otherwise, another thread can come in between and
            // 'e_WRITE_CACHE_LOWWAT' can be generated and the flag reset to
            // false BEFORE the callback is delivered.  This way, the user will
            // see the following sequence: LOWWAT, HIWAT, HIWAT (maybe),
            // LOWWAT, which is wrong, especially if no messages are queued
            // after HIWAT.
        }
        return e_HIT_CACHE_HIWAT;                                     // RETURN
    }

    if (d_recordedMaxWriteCacheSize.loadRelaxed() < writeCacheSize) {
        d_recordedMaxWriteCacheSize.storeRelaxed(writeCacheSize);
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!d_isWriteActive)) {
        // This message is the first and only in the outgoing queue.  Note that
        // if 'blob' were a 'bsl::shared_ptr<btlb::Blob> msg' instead, we could
        // simply assign 'd_writeActiveData = msg' (which would assign the
        // shared pointer to the blob) but this would have the problem that we
        // could not know if the blob of 'msg' was created with the proper blob
        // buffer factory or allocator, and also this might not preserve the
        // capacity (in buffers) of the outgoing message.  It might also
        // introduce extra (non-data) buffers into the blob of the outgoing
        // message.  Instead we want to append the blob buffers to the existing
        // outgoing message, have no non-data buffers after the last data
        // buffer, and make sure that the length is exactly the sum of all
        // buffer sizes (i.e., trim off the last data buffer).

        // Signal that there is now a message scheduled for writing.

        d_isWriteActive = true; // This must be done before releasing the lock.

        BSLS_ASSERT(0 == d_writeActiveData->numBuffers());
        BSLS_ASSERT(0 == d_writeActiveDataCurrentBuffer);
        BSLS_ASSERT(0 == d_writeActiveDataCurrentOffset);

        d_writeActiveCacheSize.addRelaxed(static_cast<int>(dataLength));

        oGuard.release()->unlock();

        // Let's first attempt to write the blob directly using iovec.

        int writeRet = MessageUtil::write(this->socket(), d_ovecs, msg);

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 < writeRet)) {
            // 'd_numBytesWritten' is modified only in the 'writeCb' or
            // 'notifyChannelDown' which will run only in the dispatcher thread
            // and when 'd_isWriteActive' is 'true'.

            d_numBytesWritten += writeRet;
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == writeRet) {
            // In case writeRet < 0, then either the socket is dead, or would
            // block if in non-blocking mode.  In the latter case, we proceed
            // to enqueue message and will retry later with writev().  For good
            // functioning of code below, though we must set writeRet to 0.

            writeRet = 0;
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // The socket is dead: close this channel for write.

            BSLS_ASSERT(btlso::SocketHandle::e_ERROR_INTERRUPTED != writeRet);

            notifyChannelDown(self, btlso::Flag::e_SHUTDOWN_SEND, false);
            return e_CHANNEL_DOWN;                                    // RETURN
        }

        d_writeActiveCacheSize.addRelaxed(-writeRet);

        if (dataLength == writeRet) {
            // We succeeded in writing the whole message.  We did release the
            // lock, however, and maybe another thread enqueued some data to
            // the outgoing blob in the meantime.  If that is the case, then,
            // we must refill the outgoing message.

            d_writeActiveData->removeAll();
            d_writeActiveDataCurrentBuffer = 0;
            d_writeActiveDataCurrentOffset = 0;

            if (!refillOutgoingMsg()) {
                // There isn't any pending data, our work here is done.

                return e_SUCCESS;                                     // RETURN
            }
        }
        else {
            // If we did not succeed, then enqueue the rest into the outgoing
            // message.  Note that 'loadBlob' will trim off the last buffer.

            int startingIndex = MessageUtil::loadBlob(d_writeActiveData.get(),
                                                      msg,
                                                      writeRet);

            d_writeActiveDataCurrentBuffer = 0;
            d_writeActiveDataCurrentOffset = startingIndex;
        }

        // There is data available, let the event manager know.

        bsl::function<void()> initWriteFunctor(bdlf::BindUtil::bind(
                                                     &Channel::registerWriteCb,
                                                      this,
                                                      self));

        d_eventManager_p->execute(initWriteFunctor);
        return e_SUCCESS;                                             // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

    // There are already outgoing messages in the 'd_writeActiveData' and
    // perhaps 'd_writeEnqueuedData', so we simply have to append those buffers
    // into 'd_writeEnqueuedData'.  Note that we are still holding the lock.

    // It is possible that a previous writeVecMessage has enqueued data, and
    // that this does not terminate on a buffer boundary.  Since we share the
    // buffers with 'blob', we should also terminate on a buffer boundary so
    // that future 'writeVecMessage' do not pollute the caller's buffers.  Note
    // that we should *never* have trailing buffers by design.

    BSLS_ASSERT(d_writeEnqueuedData->numDataBuffers() ==
                                            d_writeEnqueuedData->numBuffers());

    d_writeEnqueuedData->trimLastDataBuffer();

    MessageUtil::appendToBlob(d_writeEnqueuedData.get(), msg);

    return e_SUCCESS;
}

int Channel::setWriteCacheHiWatermark(int numBytes)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_writeMutex);

    if (d_writeCacheLowWat > numBytes) {
        return -1;                                                    // RETURN
    }

    setWriteCacheHiWatermarkRaw(numBytes);

    return 0;
}

void Channel::setWriteCacheHiWatermarkRaw(int numBytes)
{
    // Generate a 'HIWAT' alert if the new cache size limit is smaller than the
    // existing cache size and a 'HIWAT' alert has not already been generated.

    const int writeCacheSize = currentWriteCacheSize();

    if (!d_hiWatermarkHitFlag && writeCacheSize >= numBytes) {
        d_hiWatermarkHitFlag = true;
        bsl::function<void()> functor(bdlf::BindUtil::bind(
                                             &d_channelStateCb,
                                              d_channelId,
                                              d_sourceId,
                                              ChannelPool::e_WRITE_CACHE_HIWAT,
                                              d_userData));

        d_eventManager_p->execute(functor);
    }
    else if (writeCacheSize < numBytes) {
        // Otherwise, if the write cache size limit is now greater than the
        // current cache size, clear the hi-water mark hit flag so additional
        // alerts will be generated.

        d_hiWatermarkHitFlag = false;
    }

    d_writeCacheHiWat = numBytes;
}

int Channel::setWriteCacheLowWatermark(int numBytes)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_writeMutex);

    if (numBytes > d_writeCacheHiWat) {
        return -1;                                                    // RETURN
    }

    setWriteCacheLowWatermarkRaw(numBytes);

    return 0;
}

void Channel::setWriteCacheLowWatermarkRaw(int numBytes)
{
    d_writeCacheLowWat = numBytes;

    if (d_hiWatermarkHitFlag
     && (currentWriteCacheSize() <= d_writeCacheLowWat)) {

        d_hiWatermarkHitFlag = false;
        bsl::function<void()> functor(bdlf::BindUtil::bind(
                                            &d_channelStateCb,
                                             d_channelId,
                                             d_sourceId,
                                             ChannelPool::e_WRITE_CACHE_LOWWAT,
                                             d_userData));

        d_eventManager_p->execute(functor);
    }
}

void Channel::setWriteCacheWatermarks(int lowWatermark, int hiWatermark)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_writeMutex);

    if (hiWatermark < d_writeCacheLowWat) {
        setWriteCacheLowWatermarkRaw(lowWatermark);
        setWriteCacheHiWatermarkRaw(hiWatermark);
    }
    else {
        setWriteCacheHiWatermarkRaw(hiWatermark);
        setWriteCacheLowWatermarkRaw(lowWatermark);
    }
}

void Channel::resetRecordedMaxWriteCacheSize()
{
    d_recordedMaxWriteCacheSize.storeRelaxed(currentWriteCacheSize());
}

// ============================================================================
//                           COMPONENT IMPLEMENTATION
// ============================================================================

                             // -----------
                             // ChannelPool
                             // -----------

// PRIVATE MANIPULATORS
TcpTimerEventManager *ChannelPool::allocateEventManager()
{
    int numManagers = d_managers.size();
    BSLS_ASSERT(numManagers == d_config.maxThreads());

    // If there's a single thread, there's no need to choose the event manager
    // with the least usage (since there is only 1 option).

    if (1 == numManagers) {
        return d_managers[0];                                         // RETURN
    }

    int result = 0;

    // If metrics are being collected, use those metrics to determine the
    // event manager with the lowest work-load.

    if (d_collectTimeMetrics) {
        int minMetrics =
                    d_managers[result]->timeMetrics()->percentage(e_CPU_BOUND);

        minMetrics += d_managers[result]->numEvents();

        for (int i = 1; i < numManagers; ++i) {
            int currentMetrics =
                         d_managers[i]->timeMetrics()->percentage(e_CPU_BOUND);

            currentMetrics += d_managers[i]->numEvents();

            if (currentMetrics < minMetrics) {
                result = i;
                minMetrics = currentMetrics;
            }
        }
    }
    else {

        // If metrics are not being collected, choose the event manager with
        // the fewest registered events.

        int minEvents = d_managers[0]->numTotalSocketEvents();
        for (int i = 1; i < numManagers; ++i) {
            int numEvents = d_managers[i]->numTotalSocketEvents();
            if (numEvents < minEvents) {
                minEvents = numEvents;
                result    = i;
            }
        }
    }
    return d_managers[result];
}

void ChannelPool::init()
{
    // Note that, if there is a single thread, there is no need to choose the
    // event manager with the least usage (as there is only 1 option) - in that
    // case we may disable the collection of time metrics unless the client has
    // explicitly requested those metrics (i.e.
    // 'd_config.requireTimeMetrics() == false').

    bsls::AtomicOperations::initInt(&d_capacity, 0);

    d_metricsFunctor = bdlf::BindUtil::bind(&ChannelPool::metricsCb, this);

    int maxThread = d_config.maxThreads();
    BSLS_ASSERT(0 < maxThread);

    // Statically allocate all the requested managers from the get-go.  This
    // makes dynamic allocation code in allocateEventManager useless (the test
    // whether d_allocators.size() == d_config.maxThreads() will always kick
    // in).  The reason for this is that dynamic manager allocation does not
    // work very well.

    for (int i = 0; i < maxThread; ++i) {
        TcpTimerEventManager *manager =
                new (*d_allocator_p) TcpTimerEventManager(d_collectTimeMetrics,
                                                          d_allocator_p);

        if (d_startFlag) {
            bslmt::ThreadAttributes attr;
            attr.setStackSize(d_config.threadStackSize());
            manager->enable(attr);
        }
        else {
            manager->disable();
        }
        d_managers.push_back(manager);
    }

    // Initialize metrics.
    if (d_collectTimeMetrics) {
        d_metricsFunctor();
    }

    // One constructor initializes these factories, so only load
    // new objects into them if they are uninitialized.

    if (!d_writeBlobFactory) {
        d_writeBlobFactory.load(
           new (*d_allocator_p) btlb::PooledBlobBufferFactory(
                                             d_config.maxOutgoingMessageSize(),
                                             d_allocator_p),
           d_allocator_p);
    }

    if (!d_readBlobFactory) {
        d_readBlobFactory.load(
           new (*d_allocator_p) btlb::PooledBlobBufferFactory(
                                             d_config.maxIncomingMessageSize(),
                                             d_allocator_p),
           d_allocator_p);
    }
}

                                  // *** Server part ***

void ChannelPool::acceptCb(int serverId, bsl::shared_ptr<ServerState> server)
{
    // Always executed in the event manager's dispatcher thread.
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                               bslmt::ThreadUtil::self(),
                               server->d_manager_p->dispatcherThreadHandle()));

    if (server->d_isClosedFlag) {
        return;                                                       // RETURN
    }

    StreamSocket *connection;

    int status = server->d_socket_p->accept(&connection);
    if (status) {
        if (btlso::SocketHandle::e_ERROR_WOULDBLOCK  != status
         && btlso::SocketHandle::e_ERROR_INTERRUPTED != status) {
            // Deregister the socket event to avoid spewing and spinning.

            bslmt::LockGuard<bslmt::Mutex> aGuard(&d_acceptorsLock);

            if (server->d_isClosedFlag) {
                d_poolStateCb(e_ERROR_ACCEPTING, serverId, e_ALERT);
                return;                                               // RETURN
            }

            server->d_manager_p->deregisterSocketEvent(
                                                  server->d_socket_p->handle(),
                                                  btlso::EventType::e_ACCEPT);
            aGuard.release()->unlock();

            bsl::function<void()> acceptRetryFunctor(
                              bdlf::BindUtil::bind(&ChannelPool::acceptRetryCb,
                                                    this,
                                                    serverId,
                                                    server));

            if (server->d_exponentialBackoff < k_MAX_EXP_BACKOFF) {
                server->d_exponentialBackoff *= 2;
                ++server->d_exponentialBackoff;     // in case it was 0
            }

            bsls::TimeInterval exponentialDelay(
                       server->d_exponentialBackoff * k_WAIT_FOR_RESOURCES, 0);

            bsls::TimeInterval exponentialBackoff =
                                   bdlt::CurrentTime::now() + exponentialDelay;

            server->d_acceptAgainId = server->d_manager_p->registerTimer(
                                                           exponentialBackoff,
                                                           acceptRetryFunctor);

            d_poolStateCb(e_ERROR_ACCEPTING, serverId, e_ALERT);
        }
        else {
            // Ignore blocking and interrupts, but reset the exponential
            // backoff counter so that exponential backoff restarts from
            // beginning.

            server->d_exponentialBackoff = 0;
        }
        return;                                                       // RETURN
    }
    BSLS_ASSERT(connection);

    // At this point, we have a valid connection.  Reset the exponential
    // backoff counter in case we're just recovering from too many FDs, and
    // proceed.

    server->d_exponentialBackoff = 0;

    typedef btlso::StreamSocketFactoryDeleter Deleter;

    bslma::ManagedPtr<StreamSocket> socket(
                                   connection,
                                   &d_factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

    int numChannels = d_channels.length();
    if (d_config.maxConnections() <= numChannels) {
        // Too many channels, move on

        d_poolStateCb(e_CHANNEL_LIMIT, serverId, e_CRITICAL);
        return;                                                       // RETURN
    }

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    // Reserve location for new channel.  This is so we have a 'newId' to
    // pass to the channel at construction.

    ChannelHandle channelHandle;
    int newId = d_channels.add(channelHandle);
    ++numChannels;
    BSLS_ASSERT(newId);

    Channel *channelPtr = new (d_pool) Channel(&socket,
                                               newId,
                                               serverId,
                                               d_config,
                                               ChannelType::e_ACCEPTED_CHANNEL,
                                               server->d_keepHalfOpenMode,
                                               d_channelStateCb,
                                               d_blobBasedReadCb,
                                               d_writeBlobFactory.ptr(),
                                               d_readBlobFactory.ptr(),
                                               manager,
                                               this,
                                               &d_sharedPtrRepAllocator,
                                               d_allocator_p);

    channelHandle.reset(channelPtr, &d_pool, d_allocator_p);
    BSLS_ASSERT(channelHandle);

    int rc = d_channels.replace(newId, channelHandle);
    BSLS_ASSERT(0 == rc);

    // Reschedule the acceptTimeoutCb.

    if (server->d_isTimedFlag) {
        BSLS_ASSERT(server->d_timeoutTimerId);
        server->d_manager_p->deregisterTimer(server->d_timeoutTimerId);

        bsl::function<void()> acceptTimeoutFunctor(
                            bdlf::BindUtil::bind(&ChannelPool::acceptTimeoutCb,
                                                  this,
                                                  serverId,
                                                  server));

        server->d_start = bdlt::CurrentTime::now() + server->d_timeout;
        server->d_timeoutTimerId = server->d_manager_p->registerTimer(
                                                         server->d_start,
                                                         acceptTimeoutFunctor);
        BSLS_ASSERT(server->d_timeoutTimerId);
    }

    bsl::function<void()> invokeChannelUpCommand(
                                bdlf::BindUtil::bind(&Channel::invokeChannelUp,
                                                      channelPtr,
                                                      channelHandle));

    bsl::function<void()> initiateReadCommand;
    if (server->d_readEnabledFlag) {
        initiateReadCommand = bdlf::BindUtil::bind(
                                                &Channel::initiateReadSequence,
                                                 channelPtr,
                                                 channelHandle);
    }

    manager->execute(invokeChannelUpCommand);
    if (server->d_readEnabledFlag) {
        manager->execute(initiateReadCommand);
    }

    if (d_config.maxConnections() == numChannels) {
        d_poolStateCb(e_CHANNEL_LIMIT, 0, e_ALERT);
    }
}

void ChannelPool::acceptRetryCb(int                          serverId,
                                bsl::shared_ptr<ServerState> server)
{
    // Always executed in the event manager's dispatcher thread.

    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                               bslmt::ThreadUtil::self(),
                               server->d_manager_p->dispatcherThreadHandle()));

    if (server->d_isClosedFlag) {
        return;                                                       // RETURN
    }

    bsl::function<void()> acceptFunctor(
                                   bdlf::BindUtil::bind(&ChannelPool::acceptCb,
                                                         this,
                                                         serverId,
                                                         server));

    if (0 != server->d_manager_p->registerSocketEvent(
                                                  server->d_socket_p->handle(),
                                                  btlso::EventType::e_ACCEPT,
                                                  acceptFunctor)) {
        close(serverId);

        d_poolStateCb(e_ERROR_ACCEPTING, serverId, e_CRITICAL);
    }
}

void ChannelPool::acceptTimeoutCb(int                          serverId,
                                  bsl::shared_ptr<ServerState> server)
{
    // Always executed in the event manager's dispatcher thread.
    BSLS_ASSERT(server->d_isTimedFlag);
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                               bslmt::ThreadUtil::self(),
                               server->d_manager_p->dispatcherThreadHandle()));

    if (server->d_isClosedFlag) {
        return;                                                       // RETURN
    }

    bsl::function<void()> acceptTimeoutFunctor(bdlf::BindUtil::bind(
                                                 &ChannelPool::acceptTimeoutCb,
                                                  this,
                                                  serverId,
                                                  server));

    server->d_start += server->d_timeout;
    server->d_timeoutTimerId = server->d_manager_p->registerTimer(
                                                         server->d_start,
                                                         acceptTimeoutFunctor);
    BSLS_ASSERT(server->d_timeoutTimerId);

    d_poolStateCb(e_ACCEPT_TIMEOUT, serverId, e_ALERT);
}

int ChannelPool::listen(const btlso::IPv4Address&   endpoint,
                        int                         backlog,
                        int                         serverId,
                        int                         reuseAddress,
                        bool                        readEnabledFlag,
                        KeepHalfOpenMode            mode,
                        bool                        isTimedFlag,
                        const bsls::TimeInterval&   timeout,
                        const btlso::SocketOptions *socketOptions)
{
    enum {
        e_AMBIGUOUS_REUSE_ADDRESS     = -11,
        e_SET_SOCKET_OPTION_FAILED    = -10,
        e_SET_CLOEXEC_FAILED          = -9,
        e_REGISTER_FAILED             = -8,
        e_SET_NONBLOCKING_FAILED      = -7,
        e_LISTEN_FAILED               = -6,
        e_LOCAL_ADDRESS_FAILED        = -5,
        e_BIND_FAILED                 = -4,
        e_SET_OPTION_FAILED           = -3,
        e_ALLOCATE_FAILED             = -2,
        e_CAPACITY_REACHED            = -1,
        e_SUCCESS                     =  0,
        e_DUPLICATE_ID                =  1
    };

    if (socketOptions
     && !socketOptions->reuseAddress().isNull()
     && (bool) reuseAddress != socketOptions->reuseAddress().value()) {
        return e_AMBIGUOUS_REUSE_ADDRESS;                             // RETURN
    }

    bslmt::LockGuard<bslmt::Mutex> aGuard(&d_acceptorsLock);

    ServerStateMap::iterator idx = d_acceptors.find(serverId);
    if (idx != d_acceptors.end()) {
        return e_DUPLICATE_ID;                                        // RETURN
    }

    bsl::shared_ptr<ServerState> server;
    server.createInplace(d_allocator_p);
    ServerState *ss = server.get();

    ss->d_socket_p  = 0;                            // must be initialized to 0
    ss->d_factory_p = &d_factory;

    // The following members are initialized further below:
    //   - d_endpoint
    //   - d_manager_p

    ss->d_timeoutTimerId     = 0;
    ss->d_creationTime       = bdlt::CurrentTime::now();
    ss->d_start              = ss->d_creationTime;
    ss->d_timeout            = timeout;
    ss->d_acceptAgainId      = 0;
    ss->d_exponentialBackoff = false;
    ss->d_isClosedFlag       = false;
    ss->d_isTimedFlag        = isTimedFlag;
    ss->d_readEnabledFlag    = readEnabledFlag;
    ss->d_keepHalfOpenMode   = mode;

    // Open the server socket: from btlsos_tcptimedcbacceptor.  Upon early
    // return, destroying the shared ptr 'server' destroys 'ss'.  (In
    // particular, it will deallocate the socket, which is why it must be set
    // to 0 above in case we exit before 'ss->d_socket_p = serverSocket'.)

    StreamSocket *serverSocket = d_factory.allocate();
    if (!serverSocket) {
        return e_ALLOCATE_FAILED;                                     // RETURN
    }
    ss->d_socket_p = serverSocket;

    // From now on, destroying the shared ptr 'server' deallocates
    // 'serverSocket' (in dtor of 'ss') and also deallocates 'ss'.

    btlso::IPv4Address serverAddress;
    if (0 != serverSocket->setOption(btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     btlso::SocketOptUtil::k_REUSEADDRESS,
                                     !!reuseAddress)) {
        return e_SET_OPTION_FAILED;                                   // RETURN
    }

    if (socketOptions) {
        const int rc = btlso::SocketOptUtil::setSocketOptions(
                                                        serverSocket->handle(),
                                                       *socketOptions);
        if (rc) {
            return e_SET_SOCKET_OPTION_FAILED;                        // RETURN
        }
    }

    if (0 != serverSocket->bind(endpoint)) {
        return e_BIND_FAILED;                                         // RETURN
    }

    if (0 != serverSocket->localAddress(&serverAddress)) {
        return e_LOCAL_ADDRESS_FAILED;                                // RETURN
    }

    BSLS_ASSERT(serverAddress.portNumber());
    ss->d_endpoint = serverAddress;

    if (0 != serverSocket->listen(backlog)) {
        return e_LISTEN_FAILED;                                       // RETURN
    }

#ifndef BTLSO_PLATFORM_WIN_SOCKETS
        // Windows has a bug -- setting listening socket to non-blocking mode
        // will force subsequent 'accept' calls to return WSAEWOULDBLOCK *even
        // when connection is present*.

    if (0 != serverSocket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE)) {
        return e_SET_NONBLOCKING_FAILED;                              // RETURN
    }

#endif

#ifdef BSLS_PLATFORM_OS_UNIX
    // Set close-on-exec flag: this only makes sense in Unix, there is no
    // equivalent for Windows.

    int fd    = serverSocket->handle();
    int flags = fcntl(fd, F_GETFD);
    int ret   = fcntl(fd, F_SETFD, flags | FD_CLOEXEC);

    if (-1 == ret) {
        return e_SET_CLOEXEC_FAILED;                                  // RETURN
    }

#endif

    bsl::pair<ServerStateMap::iterator, bool> idx_status =
                                    d_acceptors.insert(bsl::make_pair(serverId,
                                                                      server));
    idx = idx_status.first;
    BSLS_ASSERT(idx_status.second);

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    ss->d_manager_p = manager;

    // Closely identical to allocateServer, but must execute the pool state
    // callback in the event manager's dispatcher thread.

    bsl::function<void()> acceptFunctor(bdlf::BindUtil::bind(
                                                        &ChannelPool::acceptCb,
                                                         this,
                                                         serverId,
                                                         server));

    if (0 != manager->registerSocketEvent(serverSocket->handle(),
                                          btlso::EventType::e_ACCEPT,
                                          acceptFunctor)) {
        d_acceptors.erase(idx);

        aGuard.release()->unlock();

        return e_REGISTER_FAILED;                                     // RETURN
    }

    if (isTimedFlag) {
        bsl::function<void()> acceptTimeoutFunctor(bdlf::BindUtil::bind(
                                                 &ChannelPool::acceptTimeoutCb,
                                                  this,
                                                  serverId,
                                                  server));

        ss->d_timeoutTimerId = manager->registerTimer(
                                            bdlt::CurrentTime::now() + timeout,
                                            acceptTimeoutFunctor);
        BSLS_ASSERT(ss->d_timeoutTimerId);
    }

    return e_SUCCESS;
}

                                  // *** Client part ***

void ChannelPool::connectCb(ConnectorMap::iterator idx)
{
    // Always executed in the event manager's dispatcher thread.

    int        clientId = idx->first;
    Connector& cs       = idx->second;

    bslmt::LockGuard<bslmt::Mutex>  cGuard(&d_connectorsLock);
    bslma::ManagedPtr<StreamSocket> socket(cs.d_socket.managedPtr());

    bool readEnabledFlag = cs.d_readEnabledFlag;
    bool halfOpenMode    = cs.d_keepHalfOpenMode;

    // The rest is identical to 'importCb', except that we must erase 'idx'
    // from the map of active connectors, and choose a manager for handling the
    // new channel (different from the manager handling this connection).

    TcpTimerEventManager *srcManager = cs.d_manager_p;
    d_connectors.erase(idx);

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    bslma::ManagedPtrDeleter  deleter;
    StreamSocket             *pointer = socket.release(&deleter);

    importCb(pointer,
             deleter,
             manager,
             srcManager,
             clientId,
             readEnabledFlag,
             halfOpenMode,
             false);
}

void ChannelPool::connectEventCb(ConnectorMap::iterator idx)
{
    // Called by socket even CONNECT on the underlying 'cs.d_socket'.
    // Always executed in the event manager's dispatcher thread.

    int                   clientId = idx->first;
    Connector&            cs       = idx->second;
    TcpTimerEventManager *manager  = cs.d_manager_p;

    BSLS_ASSERT(manager);
    BSLS_ASSERT(cs.d_socket);
    BSLS_ASSERT(cs.d_timeoutTimerId);
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                           manager->dispatcherThreadHandle()));

    manager->deregisterSocket(cs.d_socket->handle());

    if (0 == cs.d_socket->connectionStatus()) {
        // Successful connection.

        if (d_config.maxConnections() == d_channels.length()) {
            // Issue pool callback, close socket, and wait until next timeout.

            cs.d_socket.reset();
            cs.d_inProgress = false;

            d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);

            if (0 == cs.d_numAttempts) {
                // There is no reason to wait until next timeout to remove the
                // connector.  Do it now instead of scheduling it.

                manager->deregisterTimer(cs.d_timeoutTimerId);
                connectTimeoutCb(idx);
            }

        }
        else {
            // Disable timeout callback and connect.

            manager->deregisterTimer(cs.d_timeoutTimerId);
            cs.d_timeoutTimerId = 0;
            connectCb(idx);
        }
    }
    else {
        // Simply close socket and wait until next timeout.

        cs.d_socket.reset();

        if (0 == cs.d_numAttempts) {
            // There is no reason to wait until timeoutCb removes the
            // connector.  Do it now instead of scheduling it.

            manager->deregisterTimer(cs.d_timeoutTimerId);
            connectTimeoutCb(idx);
        }
    }
}

void ChannelPool::connectInitiateCb(ConnectorMap::iterator idx)
{
    // Always executed in the event manager's dispatcher thread.

    int                   clientId = idx->first;
    Connector&            cs       = idx->second;
    TcpTimerEventManager *manager  = cs.d_manager_p;

    BSLS_ASSERT(manager);
    BSLS_ASSERT(0 == cs.d_timeoutTimerId);
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                           manager->dispatcherThreadHandle()));

    // Decrease number of attempts, now reflecting number of calls to
    // connectInitiateCb and no longer calls to 'socket->connect()'.  Possible
    // causes for failure include getAddress, allocating socket, setting
    // non-blocking mode, or connect: if any fail, the 'continueFlag' will be
    // set to false and further logic will be skipped until rescheduling
    // timeout.  Note that WOULDBLOCK is not considered a failure, although it
    // still leads to rescheduling the timeout.

    --cs.d_numAttempts;

    bool continueFlag = true;

    if (cs.d_resolutionFlag) {
        int errorCode;
        int retCode = btlso::ResolveUtil::getAddress(&cs.d_serverAddress,
                                                     cs.d_serverName.c_str(),
                                                     &errorCode);
        if (retCode) {
            d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
            continueFlag = false;
        }
    }

    if (continueFlag && !cs.d_socket) {
        StreamSocket *connectionSocket = d_factory.allocate();

        if (connectionSocket) {
            if (0 == connectionSocket->setBlockingMode(
                                            btlso::Flag::e_NONBLOCKING_MODE)) {

                typedef btlso::StreamSocketFactory<btlso::IPv4Address> Factory;
                cs.d_socket.reset(connectionSocket,
                                  bdlf::MemFnUtil::memFn(&Factory::deallocate,
                                                         &d_factory),
                                  d_allocator_p);
            }
            else {
                d_factory.deallocate(connectionSocket);
                d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
                continueFlag = false;
            }
        }
        else {
            d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
            continueFlag = false;
        }
    }

    StreamSocket *socket = cs.d_socket.get();

    if (continueFlag && socket) {
        // At this point, the serverAddress and socket are set and valid, and
        // socket is in non-blocking mode.

        if (!cs.d_socketOptions.isNull()) {
            const int rc = btlso::SocketOptUtil::setSocketOptions(
                                                   socket->handle(),
                                                   cs.d_socketOptions.value());

            if (rc) {
                d_poolStateCb(e_ERROR_SETTING_OPTIONS, clientId, e_ALERT);

                bslmt::LockGuard<bslmt::Mutex> cGuard(&d_connectorsLock);
                d_connectors.erase(idx);
                return;                                               // RETURN
            }
        }

        // If a client address is specified bind to that address.

        if (!cs.d_localAddress.isNull()) {
            const int rc = socket->bind(cs.d_localAddress.value());

            if (rc) {
                d_poolStateCb(e_ERROR_BINDING_CLIENT_ADDR,
                              clientId,
                              e_ALERT);

                bslmt::LockGuard<bslmt::Mutex> cGuard(&d_connectorsLock);
                d_connectors.erase(idx);
                return;                                               // RETURN
            }
        }

        int retCode = socket->connect(cs.d_serverAddress);

        if (0 == retCode && 0 == socket->connectionStatus()) {
            // Since we are already in the event manager dispatcher's thread...

            if (d_config.maxConnections() == d_channels.length()) {
                // Issue pool callback and close socket.

                cs.d_socket.reset();
                cs.d_inProgress = false;

                d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
                continueFlag = false;
            }
            else {
                connectCb(idx);
                return;                                               // RETURN
            }
        }
        else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == retCode) {
          bsl::function<void()> connectEventFunctor(
                             bdlf::BindUtil::bind(&ChannelPool::connectEventCb,
                                                   this,
                                                   idx));

            // Keep guard active for makeM(&connectTimeoutFunctor, ...) below.

            cs.d_inProgress = true;
            manager->registerSocketEvent(socket->handle(),
                                         btlso::EventType::e_CONNECT,
                                         connectEventFunctor);

            // Note: continueFlag still equals true.
        }
        else {
            cs.d_socket.reset();

            d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
            continueFlag = false;
        }
    }

    if (!continueFlag && 0 == cs.d_numAttempts) {
        // Unless we are blocking in connect, there is no reason to wait until
        // timeoutCb removes the connector.  Do it now instead of scheduling.

        connectTimeoutCb(idx);
        return;                                                       // RETURN
    }

    // Reschedule timeout.
    cs.d_start += cs.d_period;

    bsl::function<void()> connectTimeoutFunctor(bdlf::BindUtil::bind(
                                                &ChannelPool::connectTimeoutCb,
                                                 this,
                                                 idx));

    cs.d_timeoutTimerId = manager->registerTimer(cs.d_start,
                                                 connectTimeoutFunctor);
    BSLS_ASSERT(cs.d_timeoutTimerId);
}

void ChannelPool::connectTimeoutCb(ConnectorMap::iterator idx)
{
    // Always executed in the event manager's dispatcher thread.
    int                   clientId = idx->first;
    Connector&            cs       = idx->second;
    TcpTimerEventManager *manager  = cs.d_manager_p;

    BSLS_ASSERT(manager);
    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(bslmt::ThreadUtil::self(),
                                           manager->dispatcherThreadHandle()));

    if (cs.d_socket) {
        manager->deregisterSocket(cs.d_socket->handle());
        cs.d_socket.reset();
    }

    // If we take the next branch, cs will become invalid.  Therefore, we need
    // to cache some of its data fields.

    const bool removeConnectorFlag = (0 == cs.d_numAttempts);
    const bool isInProgress        = cs.d_inProgress;

    if (removeConnectorFlag) {
        // Final attempt timed out.  Remove the connector before calling back
        // the client so if it picks the same "clientId", it does not get a
        // duplicate id error.

        bslmt::LockGuard<bslmt::Mutex> cGuard(&d_connectorsLock);
        d_connectors.erase(idx);
    }

    if (isInProgress) {
        // The callback was already invoked if the connection failed earlier,
        // do not invoke it twice.

        d_poolStateCb(e_ERROR_CONNECTING, clientId, e_ALERT);
    }

    if (removeConnectorFlag) {
        return;                                                       // RETURN
    }

    cs.d_timeoutTimerId = 0;
    cs.d_inProgress = false;

    // Try again.

    connectInitiateCb(idx);
}

                                  // *** Channel management part ***

void ChannelPool::importCb(StreamSocket                    *socket_p,
                           const bslma::ManagedPtrDeleter&  deleter,
                           TcpTimerEventManager            *manager,
                           TcpTimerEventManager            *srcManager,
                           int                              sourceId,
                           bool                             readEnabledFlag,
                           bool                             mode,
                           bool                             imported)
{
    bslma::ManagedPtr<StreamSocket> socket;
    if (deleter.object() == socket_p) {
        socket.load(socket_p, deleter.factory(), deleter.deleter());
    } else {
        // the ManagedPtr that originally managed this socket was an alias.
        // recreate the original alias using the information in the deleter.

        bslma::ManagedPtr<void> alias(deleter.object(),
                                      deleter.factory(),
                                      deleter.deleter());

        socket = bslma::ManagedPtr<StreamSocket>(alias, socket_p);
    }

    // Always executed in the source event manager's dispatcher thread.

    BSLS_ASSERT(bslmt::ThreadUtil::isEqual(
                                        bslmt::ThreadUtil::self(),
                                        srcManager->dispatcherThreadHandle()));

    // Reserve location for new channel.

    ChannelHandle channelHandle;
    int newId = d_channels.add(channelHandle);
    BSLS_ASSERT(newId);

    // Creating the channel first and then the shared pointer is necessary to
    // workaround a bug when using placement new as a constructor argument in
    // aCC (aC++/ANSI C B3910B A.06.00 [Aug 25 2004]).

    ChannelType::Value type = imported
                              ? ChannelType::e_IMPORTED_CHANNEL
                              : ChannelType::e_CONNECTED_CHANNEL;

    Channel *channelPtr = new (d_pool) Channel(&socket,
                                               newId,
                                               sourceId,
                                               d_config,
                                               type,
                                               mode,
                                               d_channelStateCb,
                                               d_blobBasedReadCb,
                                               d_writeBlobFactory.ptr(),
                                               d_readBlobFactory.ptr(),
                                               manager,
                                               this,
                                               &d_sharedPtrRepAllocator,
                                               d_allocator_p);

    channelHandle.reset(channelPtr, &d_pool, d_allocator_p);
    BSLS_ASSERT(channelHandle);

    int rc = d_channels.replace(newId, channelHandle);
    BSLS_ASSERT(0 == rc);

    bsl::function<void()> invokeChannelUpCommand(bdlf::BindUtil::bind(
                                                     &Channel::invokeChannelUp,
                                                      channelPtr,
                                                      channelHandle));

    bsl::function<void()> initiateReadCommand;
    if (readEnabledFlag) {
        initiateReadCommand = bdlf::BindUtil::bind(
                                                &Channel::initiateReadSequence,
                                                 channelPtr,
                                                 channelHandle);
    }

    manager->execute(invokeChannelUpCommand);

    if (readEnabledFlag) {
        manager->execute(initiateReadCommand);
    }

    if (d_config.maxConnections() == d_channels.length()) {
        d_poolStateCb(e_CHANNEL_LIMIT, sourceId, e_ALERT);
    }
}

                                  // *** Clock management ***

void ChannelPool::timerCb(int clockId)
{
    bslmt::LockGuard<bslmt::Mutex> tGuard(&d_timersLock);

    TimerStateMap::iterator tsit = d_timers.find(clockId);
    if (d_timers.end() == tsit) {
        // The timer is already deregistered.

        return;                                                       // RETURN
    }

    TimerState&           ts = tsit->second;
    bsl::function<void()> cb = ts.d_callback;

    if (ts.d_period > 0) {
        // This is a recurring clock, we must re-register a timer with the same
        // 'clockId'.

        ts.d_absoluteTime += ts.d_period;
        tGuard.release()->unlock();

        bsl::function<void()> functor(bdlf::BindUtil::bind(
                                                         &ChannelPool::timerCb,
                                                          this,
                                                          clockId));

        ts.d_eventManagerId = ts.d_eventManager_p->registerTimer(
                                                             ts.d_absoluteTime,
                                                             functor);
    }
    else {
        // This is a one-time timer, we can deregister the 'clockId'.

        d_timers.erase(tsit);
        tGuard.release()->unlock();
    }

    cb();
}

                                  // *** Metrics ***

void ChannelPool::metricsCb()
{
    int s = 0;
    BSLS_ASSERT(static_cast<int>(d_managers.size()) <= d_config.maxThreads());
    int numManagers = d_managers.size();

    BSLS_ASSERT(0 < numManagers);
    BSLS_ASSERT(0 < d_config.maxThreads());

    for (int i = 0; i < numManagers; ++i) {
        s += d_managers[i]->timeMetrics()->percentage(e_CPU_BOUND);
        d_managers[i]->timeMetrics()->resetAll();
    }

    double d = static_cast<double>(s) / d_config.maxThreads();

    BSLS_ASSERT(0 <= d);
    BSLS_ASSERT(100 >= d);

    bsls::AtomicOperations::setInt(&d_capacity, (int)d);

    d_metricsTimerId.makeValue(
        d_managers[0]->registerTimer(
                         bdlt::CurrentTime::now() + d_config.metricsInterval(),
                         d_metricsFunctor));
}

// CREATORS
ChannelPool::ChannelPool(ChannelStateChangeCallback       channelStateCb,
                         BlobBasedReadCallback            blobBasedReadCb,
                         PoolStateChangeCallback          poolStateCb,
                         const ChannelPoolConfiguration&  parameters,
                         bslma::Allocator                *basicAllocator)
: d_channels(basicAllocator)
, d_managers(basicAllocator)
, d_managersStateChangeLock()
, d_connectors(bsl::less<int>(), basicAllocator)
, d_connectorsLock()
, d_acceptors(basicAllocator)
, d_acceptorsLock()
, d_sharedPtrRepAllocator(basicAllocator)
, d_timersLock()
, d_timers(basicAllocator)
, d_config(parameters)
, d_startFlag(0)
, d_collectTimeMetrics(parameters.collectTimeMetrics())
, d_channelStateCb(channelStateCb)
, d_poolStateCb(poolStateCb)
, d_blobBasedReadCb(blobBasedReadCb)
, d_totalConnectionsLifetime(0)
, d_lastResetTime(bdlt::CurrentTime::now())
, d_totalBytesReadAdjustment(0)
, d_totalBytesWrittenAdjustment(0)
, d_totalBytesRequestedWrittenAdjustment(0)
, d_metricAdjustmentMutex()
, d_factory(basicAllocator)
, d_pool(sizeof(Channel), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init();
}

ChannelPool::ChannelPool(btlb::BlobBufferFactory         *blobBufferFactory,
                         ChannelStateChangeCallback       channelStateCb,
                         BlobBasedReadCallback            blobBasedReadCb,
                         PoolStateChangeCallback          poolStateCb,
                         const ChannelPoolConfiguration&  parameters,
                         bslma::Allocator                *basicAllocator)
: d_channels(basicAllocator)
, d_managers(basicAllocator)
, d_connectors(bsl::less<int>(), basicAllocator)
, d_acceptors(basicAllocator)
, d_sharedPtrRepAllocator(basicAllocator)
, d_writeBlobFactory(blobBufferFactory, 0, &bslma::ManagedPtrUtil::noOpDeleter)
, d_readBlobFactory(blobBufferFactory, 0, &bslma::ManagedPtrUtil::noOpDeleter)
, d_timersLock()
, d_timers(basicAllocator)
, d_config(parameters)
, d_startFlag(0)
, d_collectTimeMetrics(parameters.collectTimeMetrics())
, d_channelStateCb(channelStateCb)
, d_poolStateCb(poolStateCb)
, d_blobBasedReadCb(blobBasedReadCb)
, d_totalConnectionsLifetime(0)
, d_lastResetTime(bdlt::CurrentTime::now())
, d_totalBytesReadAdjustment(0)
, d_totalBytesWrittenAdjustment(0)
, d_totalBytesRequestedWrittenAdjustment(0)
, d_metricAdjustmentMutex()
, d_factory(basicAllocator)
, d_pool(sizeof(Channel), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    init();
}

ChannelPool::~ChannelPool()
{
    stop();
    if (!d_metricsTimerId.isNull()) {
        d_managers[0]->deregisterTimer(d_metricsTimerId.value());
    }

    // Deallocate channels.

    d_channels.removeAll();

    // Deallocate pending connecting sockets.

    d_connectors.clear();

    // Deallocate servers.

    d_acceptors.clear();

    // Deallocate event managers.

    int numEventManagers = d_managers.size();
    for (int i = 0; i < numEventManagers; ++i) {
        d_allocator_p->deleteObjectRaw(d_managers[i]);
    }
}

                       // *** Server related section ***

int ChannelPool::close(int serverId)
{
    enum {
        e_SUCCESS   =  0,
        e_NOT_FOUND = -1
    };

    bslmt::LockGuard<bslmt::Mutex> aGuard(&d_acceptorsLock);

    ServerStateMap::iterator idx = d_acceptors.find(serverId);
    if (idx == d_acceptors.end()) {
        return e_NOT_FOUND;                                           // RETURN
    }

    ServerState *ss = idx->second.get();

    // Safe even if not in the dispatcher thread, because all accesses to idx
    // (and hence the lifetime of ss) are safeguarded by the acceptors lock.

    ss->d_isClosedFlag = 1;

    // Each call below erases one reference to the shared ptr to the server
    // state, but the deregisterTimer may not succeed if the callback is
    // pending or being executed; no matter, callback cannot acquire the lock,
    // and as soon as this method returns, the flag above will make the
    // callback exit right away.  Last reference will trigger destruction of
    // the server state, i.e., deallocation of the server socket.

    if (ss->d_timeoutTimerId) {
        ss->d_manager_p->deregisterTimer(ss->d_timeoutTimerId);
        ss->d_timeoutTimerId = 0;
    }

    if (ss->d_acceptAgainId) {
        ss->d_manager_p->deregisterTimer(ss->d_acceptAgainId);
        ss->d_acceptAgainId = 0;
    }

    ss->d_manager_p->deregisterSocket(ss->d_socket_p->handle());

    d_acceptors.erase(idx);

    return e_SUCCESS;
}

int ChannelPool::listen(int                         port,
                        int                         backlog,
                        int                         serverId,
                        int                         reuseAddress,
                        bool                        readEnabledFlag,
                        const btlso::SocketOptions *socketOptions)
{
    enum { e_IS_NOT_TIMED = 0 };

    btlso::IPv4Address endpoint;
    endpoint.setPortNumber(port);

    return this->listen(endpoint,
                        backlog,
                        serverId,
                        reuseAddress,
                        readEnabledFlag,
                        e_CLOSE_BOTH,
                        e_IS_NOT_TIMED,
                        bsls::TimeInterval(),
                        socketOptions);
}

int ChannelPool::listen(int                         port,
                        int                         backlog,
                        int                         serverId,
                        const bsls::TimeInterval&   timeout,
                        int                         reuseAddress,
                        bool                        readEnabledFlag,
                        const btlso::SocketOptions *socketOptions)
{
    enum { e_IS_TIMED = 1 };

    btlso::IPv4Address endpoint;
    endpoint.setPortNumber(port);

    return this->listen(endpoint,
                        backlog,
                        serverId,
                        reuseAddress,
                        readEnabledFlag,
                        e_CLOSE_BOTH,
                        e_IS_TIMED,
                        timeout,
                        socketOptions);
}

int ChannelPool::listen(const btlso::IPv4Address&   endpoint,
                        int                         backlog,
                        int                         serverId,
                        int                         reuseAddress,
                        bool                        readEnabledFlag,
                        const btlso::SocketOptions *socketOptions)
{
    enum { e_IS_NOT_TIMED = 0 };

    return this->listen(endpoint,
                        backlog,
                        serverId,
                        reuseAddress,
                        readEnabledFlag,
                        e_CLOSE_BOTH,
                        e_IS_NOT_TIMED,
                        bsls::TimeInterval(),
                        socketOptions);
}

int ChannelPool::listen(const btlso::IPv4Address&   endpoint,
                        int                         backlog,
                        int                         serverId,
                        const bsls::TimeInterval&   timeout,
                        int                         reuseAddress,
                        bool                        readEnabledFlag,
                        KeepHalfOpenMode            mode,
                        const btlso::SocketOptions *socketOptions)
{
    enum { e_IS_TIMED = 1 };

    return this->listen(endpoint,
                        backlog,
                        serverId,
                        reuseAddress,
                        readEnabledFlag,
                        mode,
                        e_IS_TIMED,
                        timeout,
                        socketOptions);
}

                         // *** Client-related section

int ChannelPool::connect(
                    const char                *hostname,
                    int                        portNumber,
                    int                        numAttempts,
                    const bsls::TimeInterval&  interval,
                    int                        sourceId,
                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                              *socket,
                    ConnectResolutionMode      resolutionMode,
                    bool                       readEnabledFlag,
                    KeepHalfOpenMode           halfCloseMode)
{
    BSLS_ASSERT(0 < numAttempts);
    BSLS_ASSERT(bsls::TimeInterval(0) < interval || 1 == numAttempts);

    if (0 != socket
     && 0 != (*socket)->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE)) {
        return e_SET_NONBLOCKING_FAILED;                              // RETURN
    }

    return connectImp(hostname,
                      portNumber,
                      numAttempts,
                      interval,
                      sourceId,
                      socket,
                      resolutionMode,
                      readEnabledFlag,
                      halfCloseMode,
                      0,
                      0);
}

int ChannelPool::connect(
                    const btlso::IPv4Address&  serverAddress,
                    int                        numAttempts,
                    const bsls::TimeInterval&  interval,
                    int                        sourceId,
                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                              *socket,
                    bool                       readEnabledFlag,
                    KeepHalfOpenMode           mode)
{
    BSLS_ASSERT(0 < numAttempts);
    BSLS_ASSERT(bsls::TimeInterval(0) < interval || 1 == numAttempts);

    if (0 != socket
     && 0 != (*socket)->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE)) {
        return e_SET_NONBLOCKING_FAILED;                              // RETURN
    }

    return connectImp(serverAddress,
                      numAttempts,
                      interval,
                      sourceId,
                      socket,
                      readEnabledFlag,
                      mode,
                      0,
                      0);
}

int ChannelPool::connectImp(
                    const char                 *serverName,
                    int                         portNumber,
                    int                         numAttempts,
                    const bsls::TimeInterval&   interval,
                    int                         clientId,
                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                               *socket,
                    ConnectResolutionMode       resolutionMode,
                    bool                        readEnabledFlag,
                    KeepHalfOpenMode            keepHalfOpenMode,
                    const btlso::SocketOptions *socketOptions,
                    const btlso::IPv4Address   *localAddress)
{
    BSLS_ASSERT(0 == socketOptions || (0 == socket && 0 == localAddress));

    if (!d_startFlag) {
        return e_NOT_RUNNING;                                         // RETURN
    }

    bslmt::LockGuard<bslmt::Mutex> cGuard(&d_connectorsLock);

    ConnectorMap::iterator idx = d_connectors.find(clientId);
    if (idx != d_connectors.end()) {
        return e_DUPLICATE_ID;                                        // RETURN
    }

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    btlso::IPv4Address serverAddress;
    bool               resolutionFlag;

    if (e_RESOLVE_ONCE == resolutionMode) {
        int errorCode = 0;
        if (btlso::ResolveUtil::getAddress(&serverAddress,
                                           serverName,
                                           &errorCode)) {
            return e_FAILED_RESOLUTION;                               // RETURN
        }
        resolutionFlag = false;
    }
    else {
        BSLS_ASSERT(e_RESOLVE_AT_EACH_ATTEMPT == resolutionMode);
        resolutionFlag = true;
    }

    bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> > socket_sp;
    if (socket) {
        socket_sp = bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> >(
                                                     *socket,
                                                     &d_sharedPtrRepAllocator);
    }

    bsl::pair<ConnectorMap::iterator,bool> idx_status =
                 d_connectors.insert(bsl::make_pair(clientId,
                                                    Connector(socket_sp,
                                                              manager,
                                                              numAttempts,
                                                              interval,
                                                              readEnabledFlag,
                                                              keepHalfOpenMode,
                                                              socketOptions,
                                                              localAddress)));
    idx = idx_status.first;
    BSLS_ASSERT(idx_status.second);

    Connector& cs = idx->second;

    cs.d_serverName     = serverName;
    cs.d_serverAddress  = serverAddress;
    cs.d_serverAddress.setPortNumber(portNumber);  // never overwritten
    cs.d_resolutionFlag = resolutionFlag;

    cGuard.release()->unlock();

    bsl::function<void()> connectFunctor(bdlf::BindUtil::bind(
                                               &ChannelPool::connectInitiateCb,
                                               this,
                                               idx));

    manager->execute(connectFunctor);

    return e_SUCCESS;
}

int ChannelPool::connectImp(
                    const btlso::IPv4Address&   server,
                    int                         numAttempts,
                    const bsls::TimeInterval&   interval,
                    int                         clientId,
                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                               *socket,
                    bool                        readEnabledFlag,
                    KeepHalfOpenMode            mode,
                    const btlso::SocketOptions *socketOptions,
                    const btlso::IPv4Address   *localAddress)
{
    BSLS_ASSERT(0 == socketOptions || 0 == socket);

    if (!d_startFlag) {
        return e_NOT_RUNNING;                                         // RETURN
    }

    bslmt::LockGuard<bslmt::Mutex> cGuard(&d_connectorsLock);

    ConnectorMap::iterator idx = d_connectors.find(clientId);
    if (idx != d_connectors.end()) {
        return e_DUPLICATE_ID;                                        // RETURN
    }

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> > socket_sp;
    if (socket) {
        socket_sp = bsl::shared_ptr<btlso::StreamSocket<btlso::IPv4Address> >(
                                                     *socket,
                                                     &d_sharedPtrRepAllocator);
    }

    bsl::pair<ConnectorMap::iterator,bool> idx_status =
                  d_connectors.insert(bsl::make_pair(clientId,
                                                     Connector(socket_sp,
                                                               manager,
                                                               numAttempts,
                                                               interval,
                                                               readEnabledFlag,
                                                               mode,
                                                               socketOptions,
                                                               localAddress)));
    idx = idx_status.first;
    BSLS_ASSERT(idx_status.second);
    idx->second.d_serverAddress = server;

    cGuard.release()->unlock();

    bsl::function<void()> connectFunctor(bdlf::BindUtil::bind(
                                               &ChannelPool::connectInitiateCb,
                                               this,
                                               idx));

    manager->execute(connectFunctor);

    return e_SUCCESS;
}

                         // *** Channel management ***

int ChannelPool::disableRead(int channelId)
{
    enum { e_NOT_FOUND = -1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }

    Channel *channel = channelHandle.get();

    if (bslmt::ThreadUtil::isEqual(
                          bslmt::ThreadUtil::self(),
                          channel->eventManager()->dispatcherThreadHandle())) {

        channel->disableRead(channelHandle, true);
    }
    else {
        bsl::function<void()> disableReadCommand(bdlf::BindUtil::bind(
                                                         &Channel::disableRead,
                                                          channel,
                                                          channelHandle,
                                                          false));

        channel->eventManager()->execute(disableReadCommand);
    }
    return 0;
}

int ChannelPool::enableRead(int channelId)
{
    enum { e_NOT_FOUND = -1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }
    BSLS_ASSERT(channelHandle);

    Channel *channel = channelHandle.get();

    bsl::function<void()> initiateReadCommand(bdlf::BindUtil::bind(
                                                &Channel::initiateReadSequence,
                                                 channel,
                                                 channelHandle));

    channel->eventManager()->execute(initiateReadCommand);
    return 0;
}

int ChannelPool::import(
                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                     *streamSocket,
                    int               sourceId,
                    bool              readEnabledFlag,
                    KeepHalfOpenMode  mode)
{
    enum {
        e_CHANNEL_LIMIT = -1,
        e_SUCCESS       =  0
    };

    if (d_config.maxConnections() == d_channels.length()) {
        return e_CHANNEL_LIMIT;                                       // RETURN
    }

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    bslma::ManagedPtrDeleter  deleter;
    StreamSocket             *pointer = streamSocket->release(&deleter);

    bsl::function<void()> importFunctor(
                                   bdlf::BindUtil::bind(&ChannelPool::importCb,
                                                         this,
                                                         pointer,
                                                         deleter,
                                                         manager,
                                                         manager,
                                                         sourceId,
                                                         readEnabledFlag,
                                                         mode,
                                                         true));

    manager->execute(importFunctor);
    return e_SUCCESS;
}

int ChannelPool::shutdown(int channelId, ShutdownMode how)
{
    return shutdown(channelId, btlso::Flag::e_SHUTDOWN_BOTH, how);
}

int ChannelPool::shutdown(int                       channelId,
                          btlso::Flag::ShutdownType type,
                          ShutdownMode              how)
{
    BSLS_ASSERT(e_IMMEDIATE == how);

    enum {
        e_NOT_FOUND    = -1,
        e_SUCCESS      = 0,
        e_ALREADY_DEAD = 1
    };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }
    BSLS_ASSERT(channelHandle);

    ChannelDownMask channelDownMask = e_CLOSED_BOTH_MASK;
    if (btlso::Flag::e_SHUTDOWN_RECEIVE == type) {
        channelDownMask = e_CLOSED_RECEIVE_MASK;
    }
    else if (btlso::Flag::e_SHUTDOWN_SEND == type) {
        channelDownMask = e_CLOSED_SEND_MASK;
    }

    Channel *channel = channelHandle.get();
    if (channel->isChannelDown(channelDownMask)) {
        return e_ALREADY_DEAD;                                        // RETURN
    }

    channel->notifyChannelDown(channelHandle, type, false);

    return e_SUCCESS;
}

int ChannelPool::stopAndRemoveAllChannels()
{
    bslmt::LockGuard<bslmt::Mutex> managersGuard(&d_managersStateChangeLock);

    // Terminate all the worker threads ensuring that no socket event is being
    // monitored.  We keep 'd_managersStateChangeLock' locked during this
    // function so another thread does not succeed in calling 'start' before
    // this function returns.

    const int numManagers = d_managers.size();
    for (int i = 0; i < numManagers; ++i) {
        if (d_managers[i]->disable()) {
           while(--i >= 0) {
               bslmt::ThreadAttributes attr;
               attr.setStackSize(d_config.threadStackSize());

               int rc = d_managers[i]->enable(attr);
               BSLS_ASSERT(0 == rc);
           }
           return -1;                                                 // RETURN
        }
    }
    d_startFlag = 0;

    // Deallocate all servers.  Note that since we have already deregistered
    // all the timer and socket events we just need to deallocate the servers
    // closing the listening sockets.

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_acceptorsLock);
        d_acceptors.clear();
    }

    // Deallocate pending connecting sockets.

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_connectorsLock);
        d_connectors.clear();
    }

    // Remove and deallocate all channels.

    d_channels.removeAll();

    // Deregister all events associated with the event managers ensuring that
    // any held shared pointers are released.

    for (int i = 0; i < numManagers; ++i) {
        d_managers[i]->deregisterAll();
        d_managers[i]->clearExecuteQueue();
    }

    return 0;
}

int ChannelPool::setWriteCacheHiWatermark(int channelId, int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(channelHandle);

    return channelHandle->setWriteCacheHiWatermark(numBytes);
}

int ChannelPool::setWriteCacheLowWatermark(int channelId, int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(channelHandle);

    return channelHandle->setWriteCacheLowWatermark(numBytes);
}

int ChannelPool::setWriteCacheWatermarks(int channelId,
                                         int lowWatermark,
                                         int hiWatermark)
{
    BSLS_ASSERT(0 <= lowWatermark);
    BSLS_ASSERT(lowWatermark <= hiWatermark);

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(channelHandle);

    channelHandle->setWriteCacheWatermarks(lowWatermark, hiWatermark);

    return 0;
}

int ChannelPool::resetRecordedMaxWriteCacheSize(int channelId)
{
    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(channelHandle);

    channelHandle->resetRecordedMaxWriteCacheSize();

    return 0;
}

                         // *** Thread management ***

int ChannelPool::start()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_managersStateChangeLock);

    int numManagers = d_managers.size();
    for (int i = 0; i < numManagers; ++i) {
        bslmt::ThreadAttributes attr;
        attr.setStackSize(d_config.threadStackSize());
        int ret = d_managers[i]->enable(attr);
        if (0 != ret) {
           while(--i >= 0) {
               int rc = d_managers[i]->disable();
               BSLS_ASSERT(0 == rc);
           }
           return ret;                                                // RETURN
        }
    }
    d_startFlag = 1;
    return 0;
}

int ChannelPool::stop()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_managersStateChangeLock);

    int numManagers = d_managers.size();
    for (int i = 0; i < numManagers; ++i) {
        if (d_managers[i]->disable()) {
           while(--i >= 0) {
               bslmt::ThreadAttributes attr;
               attr.setStackSize(d_config.threadStackSize());
               int rc = d_managers[i]->enable(attr);
               BSLS_ASSERT(0 == rc);
           }
           return -1;                                                 // RETURN
        }
    }
    d_startFlag = 0;
    return 0;
}

                         // *** Outgoing messages ***

void ChannelPool::setChannelContext(int channelId, void *context)
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        channelHandle->setUserData(context);
    }
}

int ChannelPool::write(int               channelId,
                       const btlb::Blob& blob,
                       int               enqueueWatermark)
{
    enum { e_NOT_FOUND = -5 };

    ChannelHandle channelHandle;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                          0 != findChannelHandle(&channelHandle, channelId))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return e_NOT_FOUND;                                           // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                          !channelHandle->isChannelDown(e_CLOSED_SEND_MASK))) {
        return channelHandle->writeMessage(blob,
                                           enqueueWatermark,
                                           channelHandle);            // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return e_NOT_FOUND;
}

int ChannelPool::write(int channelId, const btls::Iovec vecs[], int numVecs)
{
    enum { e_NOT_FOUND = -5 };

    ChannelHandle channelHandle;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                          0 != findChannelHandle(&channelHandle, channelId))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return e_NOT_FOUND;                                           // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                          !channelHandle->isChannelDown(e_CLOSED_SEND_MASK))) {
        return channelHandle->writeMessage(
                    ChannelPool_IovecArray<btls::Iovec>(vecs, numVecs),
                    0x7FFFFFFF,
                    channelHandle);                                   // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return e_NOT_FOUND;
}

int ChannelPool::write(int channelId, const btls::Ovec vecs[], int numVecs)
{
    enum { e_NOT_FOUND = -5 };

    ChannelHandle channelHandle;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                          0 != findChannelHandle(&channelHandle, channelId))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return e_NOT_FOUND;                                           // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                          !channelHandle->isChannelDown(e_CLOSED_SEND_MASK))) {
        return channelHandle->writeMessage(
                    ChannelPool_IovecArray<btls::Ovec>(vecs, numVecs),
                    0x7FFFFFFF,
                    channelHandle);                                   // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return e_NOT_FOUND;
}

                          // *** Clock management ***

int ChannelPool::registerClock(const bsl::function<void()>& command,
                               const bsls::TimeInterval&    startTime,
                               const bsls::TimeInterval&    period,
                               int                          clockId)
{
    enum {
        e_SUCCESS      = 0,
        e_DUPLICATE_ID = 1
    };

    TcpTimerEventManager *manager = allocateEventManager();
    BSLS_ASSERT(manager);

    TimerState ts;
    ts.d_absoluteTime   = startTime;
    ts.d_period         = period;
    ts.d_eventManager_p = manager;
    ts.d_callback       = command;

    bsl::function<void()> functor(bdlf::BindUtil::bind(&ChannelPool::timerCb,
                                                        this,
                                                        clockId));

    bslmt::LockGuard<bslmt::Mutex> tGuard(&d_timersLock);
    if (d_timers.end() != d_timers.find(clockId)) {
        return e_DUPLICATE_ID;                                        // RETURN
    }

    bsl::pair<TimerStateMap::iterator,bool> tsp = d_timers.insert(
                                                  bsl::make_pair(clockId, ts));
    BSLS_ASSERT(tsp.second);

    tsp.first->second.d_eventManagerId = manager->registerTimer(startTime,
                                                                functor);
    return e_SUCCESS;
}

int ChannelPool::registerClock(const bsl::function<void()>& command,
                               const bsls::TimeInterval&    startTime,
                               const bsls::TimeInterval&    period,
                               int                          clockId,
                               int                          channelId)
{
    enum {
        e_SUCCESS            = 0,
        e_DUPLICATE_ID       = 1,
        e_INVALID_CHANNEL_ID = 2
    };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_INVALID_CHANNEL_ID;                                  // RETURN
    }

    TcpTimerEventManager *manager = channelHandle->eventManager();
    TimerState            ts;
    ts.d_absoluteTime   = startTime;
    ts.d_period         = period;
    ts.d_eventManager_p = manager;
    ts.d_callback       = command;

    bsl::function<void()> functor(bdlf::BindUtil::bind(&ChannelPool::timerCb,
                                                        this,
                                                        clockId));

    bslmt::LockGuard<bslmt::Mutex> tGuard(&d_timersLock);
    if (d_timers.end() != d_timers.find(clockId)) {
        return e_DUPLICATE_ID;                                        // RETURN
    }

    bsl::pair<TimerStateMap::iterator,bool> tsp =
                                  d_timers.insert(bsl::make_pair(clockId, ts));
    BSLS_ASSERT(tsp.second);

    tsp.first->second.d_eventManagerId = manager->registerTimer(startTime,
                                                                functor);
    return e_SUCCESS;
}

void ChannelPool::deregisterClock(int clockId)
{
    TcpTimerEventManager *manager = 0;
    void                 *timerId = 0;
    {
        bslmt::LockGuard<bslmt::Mutex> tGuard1(&d_timersLock);
        TimerStateMap::iterator        tsit = d_timers.find(clockId);

        if (d_timers.end() == tsit) {
            return;                                                   // RETURN
        }

        manager = tsit->second.d_eventManager_p;
        timerId = tsit->second.d_eventManagerId;
    }

    manager->deregisterTimer(timerId);

    // Event manager guarantees that timer callback is never invoked after
    // 'deregisterTimer' returns.

    {
        bslmt::LockGuard<bslmt::Mutex> tGuard2(&d_timersLock);
        TimerStateMap::iterator        tsit = d_timers.find(clockId);
        if (d_timers.end() == tsit) {
            // We could be racing against another thread, and the previous
            // value of 'tsit' may no longer be valid.  This is not an error.

            return;                                                   // RETURN
        }
        d_timers.erase(tsit);
    }
}

                           // *** Socket options ***

int ChannelPool::getLingerOption(
                             btlso::SocketOptUtil::LingerData *result,
                             int                               channelId) const
{
    enum  { e_NOT_FOUND = 1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }
    return channelHandle->socket()->lingerOption(result);
}

int
ChannelPool::getServerSocketOption(int *result,
                                   int  option,
                                   int  level,
                                   int  serverId) const
{
    enum { e_NOT_FOUND = 1 };

    bslmt::LockGuard<bslmt::Mutex> dGuard(&d_acceptorsLock);

    ServerStateMap::const_iterator idx = d_acceptors.find(serverId);
    if (idx == d_acceptors.end()) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return idx->second->d_socket_p->socketOption(result, level, option);
}

int ChannelPool::getSocketOption(int *result,
                                 int  option,
                                 int  level,
                                 int  channelId) const
{
    BSLS_ASSERT(result);

    enum { e_NOT_FOUND = 1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return channelHandle->socket()->socketOption(result, level, option);
}

int ChannelPool::setLingerOption(
                             const btlso::SocketOptUtil::LingerData& value,
                             int                                     channelId)
{
    enum  { e_NOT_FOUND = 1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return channelHandle->socket()->setLingerOption(value);
}

int ChannelPool::setServerSocketOption(int option,
                                       int level,
                                       int value,
                                       int serverId)
{
    enum  { e_NOT_FOUND = 1 };

    bslmt::LockGuard<bslmt::Mutex> dGuard(&d_acceptorsLock);

    ServerStateMap::const_iterator idx = d_acceptors.find(serverId);
    if (idx == d_acceptors.end()) {
        return e_NOT_FOUND;                                           // RETURN
    }
    BSLS_ASSERT(idx->second->d_socket_p);

    return idx->second->d_socket_p->setOption(level, option, value);
}

int ChannelPool::setSocketOption(int option,
                                 int level,
                                 int value,
                                 int channelId)
{
    enum  { e_NOT_FOUND = 1 };

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return e_NOT_FOUND;                                           // RETURN
    }

    return channelHandle->socket()->setOption(level, option, value);
}

                              // *** Metrics ***

double ChannelPool::reportWeightedAverageReset()
{
    bsls::TimeInterval resetTime = d_lastResetTime;
    d_lastResetTime = bdlt::CurrentTime::now();
    bsls::Types::Int64 sum = d_totalConnectionsLifetime.swap(0);

    for (bdlcc::ObjectCatalogIter<ChannelHandle > it(d_channels); it; ++it) {
        bsl::pair<int, ChannelHandle> p = it();

        if (!p.second || p.second->isChannelDown(e_CLOSED_BOTH_MASK)) {
            continue;
        }

        bsls::TimeInterval intv = d_lastResetTime -
                                          (resetTime > p.second->creationTime()
                                          ? resetTime
                                          : p.second->creationTime());

        sum += intv.totalMicroseconds();
    }
    bsls::TimeInterval intv = d_lastResetTime - resetTime;
    bsls::Types::Int64 timeDiff = intv.totalMicroseconds();

    return timeDiff > 0
           ? static_cast<double>(sum) / static_cast<double>(timeDiff)
           : -1;
}

void ChannelPool::totalBytesReadReset(bsls::Types::Int64 *result)
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex>          guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);

    bsls::Types::Int64 total = 0;
    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesRead();
        }
    }
    *result = d_totalBytesReadAdjustment + total;
    d_totalBytesReadAdjustment = -total;
}

void ChannelPool::totalBytesWrittenReset(bsls::Types::Int64 *result)
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex> guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);

    bsls::Types::Int64 total = 0;
    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesWritten();
        }
    }
    *result = d_totalBytesWrittenAdjustment + total;
    d_totalBytesWrittenAdjustment = -total;
}

void ChannelPool::totalBytesRequestedToBeWrittenReset(
                                                    bsls::Types::Int64 *result)
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex> guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);

    bsls::Types::Int64 total = 0;
    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesRequestedToBeWritten();
        }
    }
    *result = d_totalBytesRequestedWrittenAdjustment + total;
    d_totalBytesRequestedWrittenAdjustment = -total;
}

// ACCESSORS

void *ChannelPool::channelContext(int channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        return channelHandle->userData();                             // RETURN
    }
    return 0;
}

bsl::shared_ptr<const btlso::StreamSocket<btlso::IPv4Address> >
ChannelPool::streamSocket(int channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        bsl::shared_ptr<const btlso::StreamSocket<btlso::IPv4Address> >
                                                  ptr(channelHandle,
                                                      channelHandle->socket());
        return ptr;                                                   // RETURN
    }
    return bsl::shared_ptr<const btlso::StreamSocket<btlso::IPv4Address> >();
}

int ChannelPool::getChannelStatistics(
                                   bsls::Types::Int64 *numRead,
                                   bsls::Types::Int64 *numRequestedToBeWritten,
                                   bsls::Types::Int64 *numWritten,
                                   int                 channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        Channel *channel = channelHandle.get();

        *numRead                 = channel->numBytesRead();
        *numRequestedToBeWritten = channel->numBytesRequestedToBeWritten();
        *numWritten              = channel->numBytesWritten();

        return 0;                                                     // RETURN
    }
    return 1;
}

int ChannelPool::getChannelWriteCacheStatistics(int *recordedMaxWriteCacheSize,
                                                int *currentWriteCacheSize,
                                                int  channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        Channel *channel = channelHandle.get();

        *recordedMaxWriteCacheSize = channel->recordedMaxWriteCacheSize();
        *currentWriteCacheSize     = channel->currentWriteCacheSize();

        return 0;                                                     // RETURN
    }
    return 1;
}

void ChannelPool::getHandleStatistics(
                                     bsl::vector<HandleInfo> *handleInfo) const
{
    // There are five kinds of channels, but only three matter for the
    // implementation:
    // 1. The server channels that are listening (ServerMap)
    // 2. Those being in connection (ConnectorMap)
    // 3. The active channels (either imported, connected, or accepted, found
    //    in the 'd_channels' catalog)

    int idx = handleInfo->size();

    {
        // Item 1.  Oops: misses sockets after accept(&connection) but before
        // the d_channels.add(channelHandle).  OK.

        bslmt::LockGuard<bslmt::Mutex> dGuard(&d_acceptorsLock);

        ServerStateMap::const_iterator iter = d_acceptors.begin();
        ServerStateMap::const_iterator last = d_acceptors.end();

        handleInfo->resize(idx + d_acceptors.size());
        for (; iter != last; ++iter, ++idx) {
            HandleInfo&        info = (*handleInfo)[idx];
            const ServerState& ss   = *(iter->second);

            // Because we hold the 'd_acceptorsLock', it's impossible that
            // 'ss.d_socket_p' could be 0 as is is set once and for all in
            // 'listen()' under the lock.

            info.d_handle       = ss.d_socket_p->handle();
            info.d_channelType  = ChannelType::e_LISTENING_CHANNEL;
            info.d_channelId    = -1;
            info.d_creationTime = ss.d_creationTime;
            info.d_threadHandle = ss.d_manager_p->dispatcherThreadHandle();
            info.d_userId       = iter->first;
        }
    }

    {
        // Item 2.  Oops: misses sockets after d_connectors.erase(idx) but
        // before the d_channels.add(channelHandle).  Might be OK.

        bslmt::LockGuard<bslmt::Mutex> dGuard(&d_connectorsLock);

        ConnectorMap::const_iterator iter = d_connectors.begin();
        ConnectorMap::const_iterator last = d_connectors.end();

        handleInfo->reserve(idx + d_connectors.size());
        for (; iter != last; ++iter) {
            const Connector& cs = iter->second;

            if (cs.d_socket) {
                handleInfo->resize(++idx);
                HandleInfo& info = handleInfo->back();

                info.d_handle       = cs.d_socket->handle();
                info.d_channelType  = ChannelType::e_CONNECTING_CHANNEL;
                info.d_channelId    = -1;
                info.d_creationTime = cs.d_creationTime;
                info.d_threadHandle = cs.d_manager_p->dispatcherThreadHandle();
                info.d_userId       = iter->first;
            }
        }
    }

    {
        // Item 3.

        handleInfo->reserve(idx + d_channels.length());
        for (bdlcc::ObjectCatalogIter<bsl::shared_ptr<Channel> >
                                              iter(d_channels); iter; ++iter) {
            if (iter().second) {
                const Channel& channel = *(iter().second);
                handleInfo->resize(++idx);
                HandleInfo& info = handleInfo->back();

                info.d_handle       = channel.socket()->handle();
                info.d_channelType  = channel.channelType();
                info.d_channelId    = channel.channelId();
                info.d_creationTime = channel.creationTime();
                info.d_threadHandle = channel.eventManager()->
                                                      dispatcherThreadHandle();
                info.d_userId       = channel.sourceId();
            }
        }
    }
}

int
ChannelPool::getServerAddress(btlso::IPv4Address *result,
                              int                 serverId) const
{
    bslmt::LockGuard<bslmt::Mutex> aGuard(&d_acceptorsLock);

    ServerStateMap::const_iterator idx = d_acceptors.find(serverId);
    if (idx == d_acceptors.end()) {
        return -1;                                                    // RETURN
    }

    *result = idx->second->d_endpoint;
    return 0;
}

int
ChannelPool::getLocalAddress(btlso::IPv4Address *result,
                             int                 channelId) const
{
    BSLS_ASSERT(result);

    ChannelHandle channelHandle;
    if (0 != findChannelHandle(&channelHandle, channelId)) {
        return -1;                                                    // RETURN
    }

    return channelHandle->socket()->localAddress(result);
}

int
ChannelPool::getPeerAddress(btlso::IPv4Address *result,
                            int                 channelId) const
{
    BSLS_ASSERT(result);

    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        *result = channelHandle->peerAddress();
        return 0;                                                     // RETURN
    }
    return 1;
}

int ChannelPool::numBytesRead(bsls::Types::Int64 *result,
                              int                 channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        *result = channelHandle->numBytesRead();
        return 0;                                                     // RETURN
    }
    return 1;
}

int ChannelPool::numBytesRequestedToBeWritten(
                                           bsls::Types::Int64 *result,
                                           int                 channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        *result = channelHandle->numBytesRequestedToBeWritten();
        return 0;                                                     // RETURN
    }
    return 1;
}

int ChannelPool::numBytesWritten(bsls::Types::Int64 *result,
                                 int                 channelId) const
{
    ChannelHandle channelHandle;
    if (0 == findChannelHandle(&channelHandle, channelId)) {
        *result = channelHandle->numBytesWritten();
        return 0;                                                     // RETURN
    }
    return 1;
}

void ChannelPool::totalBytesWritten(bsls::Types::Int64 *result) const
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex>          guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);
    bsls::Types::Int64                      total = 0;

    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesWritten();
        }
    }
    *result = total + d_totalBytesWrittenAdjustment;
}

void ChannelPool::totalBytesRead(bsls::Types::Int64 *result) const
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex>          guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);
    bsls::Types::Int64                      total = 0;

    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesRead();
        }
    }
    *result = total + d_totalBytesReadAdjustment;
}

void ChannelPool::totalBytesRequestedToBeWritten(
                                              bsls::Types::Int64 *result) const
{
    // Note that this lock must be held to ensure that updating the adjustment
    // to the metric total, and removing the channel is handled atomically.

    bslmt::LockGuard<bslmt::Mutex>          guard(&d_metricAdjustmentMutex);
    bdlcc::ObjectCatalogIter<ChannelHandle> it(d_channels);
    bsls::Types::Int64                      total = 0;

    for (; it; ++it) {
        if (it().second) {
            total += it().second->numBytesRequestedToBeWritten();
        }
    }
    *result = total + d_totalBytesRequestedWrittenAdjustment;
}

int ChannelPool::numEvents(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(static_cast<int>(d_managers.size()) > index);

    return d_managers[index]->numEvents();
}

                     // -----------------------------
                     // class ChannelPool_MessageUtil
                     // -----------------------------

// CLASS METHODS
int ChannelPool_MessageUtil::loadIovec(btls::Iovec       *dest,
                                       const btlb::Blob&  msg)
{
    int       numVecs        = 0;
    const int numDataBuffers = msg.numDataBuffers();
    int       numBuffers     = (k_MAX_IOVEC_SIZE < numDataBuffers)
                               ? k_MAX_IOVEC_SIZE
                               : numDataBuffers;

    for (int i = 0; i < numBuffers; ++i, ++numVecs) {
        int bufSize = i < numDataBuffers - 1
                      ? msg.buffer(i).size()
                      : msg.lastDataBufferLength();

        dest[numVecs].setBuffer(msg.buffer(i).data(), bufSize);
    }

    return numVecs;
}

int ChannelPool_MessageUtil::loadBlob(btlb::Blob        *dest,
                                      const btlb::Blob&  msg,
                                      int                msgOffset)
{
    BSLS_ASSERT(0 == dest->length());

    const int numDataBuffers = msg.numDataBuffers();
    int       bufIdx         = 0;
    int       prefixSize     = 0;

    // Because each buffer may be of different size, we must walk the blob to
    // find the buffer containing 'msgOffset'.

    while (bufIdx < numDataBuffers
        && prefixSize + msg.buffer(bufIdx).size() <= msgOffset) {
        // We don't need to append those buffers, all their data falls before
        // 'startingIndex'.

        prefixSize += msg.buffer(bufIdx).size();
        ++bufIdx;
    }

    while (bufIdx < numDataBuffers) {
        dest->appendDataBuffer(msg.buffer(bufIdx));
        ++bufIdx;
    }

    // Now that all the buffers have been appended, setLength should not
    // trigger a buffer allocation.

    dest->setLength(msg.length() - prefixSize);
    dest->trimLastDataBuffer();

    return msgOffset - prefixSize;
}

void ChannelPool_MessageUtil::appendToBlob(btlb::Blob        *dest,
                                           const btlb::Blob&  msg)
{
    const int currentLength  = dest->length();
    const int numDataBuffers = msg.numDataBuffers();

    for (int bufIdx = 0; bufIdx < numDataBuffers; ++bufIdx) {
        dest->appendDataBuffer(msg.buffer(bufIdx));
    }

    // Now that all the buffers have been appended, setLength should not
    // trigger a buffer allocation.

    dest->setLength(currentLength + msg.length());
    dest->trimLastDataBuffer();
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
