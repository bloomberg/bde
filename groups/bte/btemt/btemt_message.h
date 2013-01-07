// btemt_message.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_MESSAGE
#define INCLUDED_BTEMT_MESSAGE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide encapsulated type-safe data and status messages.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btemt_MessageImpl: raw data container
//      btemt_BlobMsg: data message represented as a 'bcema_Blob'
//   btemt_ChannelMsg: channel state message
//      btemt_DataMsg: data message represented as a 'PooledBufferChain'
//      btemt_PoolMsg: pool state message
//     btemt_TimerMsg: timer message
//      btemt_UserMsg: user message
//      btemt_Message: container for a generic message
//
//@SEE_ALSO: btemt_channelqueuepool btemt_channelpool btemt_sessionpool
//
//@DESCRIPTION: This component provides a suite of attribute classes
// that represent various messages emitted by queue-based channel
// pools.  The messages are classified as :
//..
//   data messages          -- contain raw data as read from a socket
//   channel state messages -- contain a new state for a channel
//   pool state messages    -- contain an event from the pool
//   timer messages         -- notification that a timer is expired.
//   user messages          -- user-generated messages
//..
// In addition to the above "specific" messages, this component provides a
// "generic" message class, 'btemt_Message', which is a uniform container for
// any one of the above five specific message types.  The class 'btemt_Message'
// provides a set of methods, each of which returns a reference to a specific
// message type; the method call must be appropriate for the contained specific
// message type.  A foolproof way to achieve that is to create a specific
// message instance (e.g., of type 'btemt_DataMsg') and create a
// 'btemt_Message' object using an initializing constructor taking the specific
// message instance.  Another way is to create a 'btemt_Message' object of a
// given specific type, and then assign the value to the reference to that
// specific message type.  The usage example illustrates both ways.
//
// Some of the message types, namely 'btemt_ChannelMsg', 'btemt_PoolMsg', and
// 'btemt_TimerMsg', are *value* *semantic* types.  The value semantics are
// defined in the corresponding 'operator=='.  The corresponding methods and
// streaming operators are provided.
//
///Thread-safety
///-------------
// Each class provided by this component is *thread* *safe*.  None of the
// classes provided by this component is *thread* *enabled*.
//
///Usage Example
///-------------
// It is possible to initialize a 'btemt_Message' object to a value of one of
// the specific-message types defined in this component (e.g.,
// 'btemt_ChannelMsg').  (See the Usage Example of 'btemt_channelqueuepool' for
// additional examples.)  First, create a channel message object
// 'channelMessage' initialized with arbitrary data:
//..
//  btemt_ChannelMsg::ChannelState state   =
//                                          btemt_ChannelMsg::BTEMT_CHANNEL_UP;
//  int                            chanId  = 123;
//  int                            allocId = 456;
//
//  btemt_ChannelMsg channelMessage(state, chanId, allocId);
//..
// We can now initialize a newly created object 'message' of type
// 'btemt_Message' with the value of 'channelMessage', and use 'message' to
// discover the attributes of 'channelMessage'.
//..
//  btemt_Message message1 = channelMessage; // calls initializing constructor
//
//  assert(btemt_Message::BTEMT_CHANNEL_STATE == message1.type());
//  assert(btemt_ChannelMsg::BTEMT_CHANNEL_UP
//             == message1.channelMsg().channelState());
//  assert(123 == message1.channelMsg().channelId());
//  assert(456 == message1.channelMsg().allocatorId());
//..
// Another way is to use the 'btemt_Message' constructor taking a
// 'btemt_Message::MessageType' argument to construct a message representing
// the indicated type and then use that message, e.g., via a specific-message
// reference, to manipulate the "contained" specific message.  For example,
// using the previous definitions:
//..
//  btemt_Message message2(btemt_Message::BTEMT_CHANNEL_STATE);
//  btemt_ChannelMsg &channelMsg = message2.channelMsg();
//
//  channelMsg.setChannelState(state);
//  channelMsg.setChannelId(chanId);
//  channelMsg.setAllocatorId(allocId);
//..
// Note that the channel messages created in either way should have the same
// type and value:
//..
//  assert(btemt_Message::BTEMT_CHANNEL_STATE == message2.type());
//  assert(message1.channelMsg() == message2.channelMsg());
//..
// The second way presented above is slightly more error-prone in case the
// 'message2' is initialized with the wrong message type.  The user should make
// sure that specific-message references are always used in conjunction with a
// valid 'btemt_Message' of the same type.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                          // =======================
                          // class btemt_MessageImpl
                          // =======================

struct btemt_MessageImpl {
    // This 'struct' is a reference-counted data holder for concrete messages.
    // Concrete message types use this 'struct' as a container, inducing
    // specific meanings on the (public) data members as appropriate.

    // TYPES
    typedef bcema_SharedPtr<bcema_PooledBufferChain> Handle;

    // INSTANCE DATA
    union {
        // Union of the data items required to represent each message type.

        char                                d_arena[sizeof(Handle)];
        void                               *d_opaque;
        bsls_AlignmentUtil::MaxAlignedType  d_dummy;
    } d_data;

    int                       d_dataLength;
    int                       d_bufferLength;
    int                       d_channelId;

    // CREATORS
    ~btemt_MessageImpl() { }
};

                           // ===================
                           // class btemt_BlobMsg
                           // ===================

class btemt_BlobMsg {
    // This class provides data associated with a given channel.  The data
    // segment can be passed as a managed or unmanaged pointer to a
    // 'bcema_Blob'.  In either case, this class shares management of the data
    // segment.

    // TYPES
    typedef bcema_SharedPtr<bcema_Blob> Handle;

    // PRIVATE INSTANCE DATA
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_BlobMsg();
        // Create an empty data message.

    btemt_BlobMsg(bcema_Blob                *blob,
                  int                        channelId,
                  bslma_Allocator           *basicAllocator = 0);
        // Create a data message containing the specified 'blob' and associated
        // with the channel having the specified 'channelId'.  This message
        // will assume ownership of 'blob' and will destroy it when
        // appropriate.  Optionally specify a 'basicAllocator' which was used
        // to allocate 'blob', which will be used to construct a shared pointer
        // and eventually deallocate 'blob'.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    btemt_BlobMsg(const bcema_SharedPtr<bcema_Blob>& dataPtr,
                  int                                channelId);
        // Create a data message sharing management of the specified 'dataPtr'
        // and associated with the channel having the specified 'channelId'.

    btemt_BlobMsg(const btemt_BlobMsg& original);
        // Create a data message referring to the same channel and data
        // segment, if any, as the specified 'original' message.

    ~btemt_BlobMsg();

    // MANIPULATORS
    btemt_BlobMsg& operator=(const btemt_BlobMsg& rhs);
        // Assign to this data message the channel ID of, and start sharing
        // management of the data segment referred to by, the specified 'rhs'
        // message, and release from management by this message any data
        // segment currently managed by this message, if any.

    void setChannelId(int channelId);
        // Set the channel ID of this data message to the specified
        // 'channelId'.

    void setData(bcema_Blob                *blob,
                 bslma_Allocator           *basicAllocator = 0);
        // Set the data segment for this data message to the specified 'blob'
        // and assume ownership of 'blob', and release from management by this
        // message any data segment currently managed by this message, if any.

    void setSharedData(const bcema_SharedPtr<bcema_Blob>& dataPtr);
        // Set the data content of this data message to the specified shared
        // 'dataPtr', and share the management of 'dataPtr'.  Also release from
        // management by this message any data segment currently managed by
        // this message, if any.

    void setUserDataField1(int value);
        // Set the user data field 1 of this data message to the specified
        // 'value'.

    void setUserDataField2(int value);
        // Set the user data field 1 of this data message to the specified
        // 'value'.

    // ACCESSORS
    int channelId() const;
        // Return the channel ID of this data message.

    bcema_Blob *data() const;
        // Return the data content of this data message represented as a
        // blob.

    bcema_SharedPtr<bcema_Blob>& sharedData() const;
        // Return the data content of this data message represented as a
        // shared pointer to a 'bcema_Blob'.

    int userDataField1() const;
        // Return the value in the user data field 1 of this data message.

    int userDataField2() const;
        // Return the value in the user data field 2 of this data message.
};

                          // ======================
                          // class btemt_ChannelMsg
                          // ======================

class btemt_ChannelMsg {
    // This class provides information about a single event on a communication
    // channel.  The channel ID and an allocator ID (of a channel acting as the
    // parent, i.e., acceptor or connector) are provided, as well as a channel
    // state.

  public:
    // ENUMERATIONS
    enum ChannelState {
        BTEMT_CHANNEL_DOWN = 0    // the connection is broken
      , BTEMT_CHANNEL_UP          // the connection is established
      , BTEMT_READ_TIMEOUT        // timed out waiting for incoming data
      , BTEMT_SEND_BUFFER_FULL    // the outgoing buffer is full *OBSOLETE*
      , BTEMT_MESSAGE_DISCARDED   // message is discarded (can't be sent)
      , BTEMT_AUTO_READ_ENABLED   // automatic reading is enabled on a channel
      , BTEMT_AUTO_READ_DISABLED  // automatic reading is disabled on a channel
      , BTEMT_WRITE_CACHE_LOWWAT  // write cache low watermark
      , BTEMT_WRITE_CACHE_HIWAT   // write cache hi watermark
          = BTEMT_SEND_BUFFER_FULL
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , CHANNEL_DOWN       = BTEMT_CHANNEL_DOWN
      , CHANNEL_UP         = BTEMT_CHANNEL_UP
      , READ_TIMEOUT       = BTEMT_READ_TIMEOUT
      , SEND_BUFFER_FULL   = BTEMT_SEND_BUFFER_FULL
      , MESSAGE_DISCARDED  = BTEMT_MESSAGE_DISCARDED
      , AUTO_READ_ENABLED  = BTEMT_AUTO_READ_ENABLED
      , AUTO_READ_DISABLED = BTEMT_AUTO_READ_DISABLED
      , WRITE_CACHE_LOWWAT = BTEMT_WRITE_CACHE_LOWWAT
      , WRITE_CACHE_HIWAT  = BTEMT_WRITE_CACHE_HIWAT
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
    enum {
        BTEMT_INVALID_ID = -1     // invalid channel and channel allocator ID
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INVALID_ID = BTEMT_INVALID_ID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // PRIVATE INSTANCE DATA
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_ChannelMsg();
        // Create a channel state message.  The channel ID and the
        // allocator ID are set to 'BTEMT_INVALID_ID', and the channel state is
        // set to 'BTEMT_CHANNEL_DOWN'.

    btemt_ChannelMsg(ChannelState state, int channelId, int allocatorId);
        // Create a channel state message having the specified 'state' related
        // to the channel with the specified 'channelId' that was allocated
        // from an allocator with the specified 'allocatorId'.

    btemt_ChannelMsg(const btemt_ChannelMsg& original);
        // Create a channel message initialized with the value of the specified
        // 'original' channel message.

    ~btemt_ChannelMsg();
        // Destroy this channel message.

    // MANIPULATORS
    btemt_ChannelMsg& operator=(const btemt_ChannelMsg& rhs);
        // Assign to this channel message the value of the specified 'rhs'
        // message and return a reference to this modifiable message.

    void setAllocatorId(int id);
        // Set the allocator ID in this channel message to the specified 'id'.

    void setChannelId(int id);
        // Set the channel ID in this channel message to the specified 'id'.

    void setChannelState(ChannelState event);
        // Set the channel state in this channel message to the specified
        // 'event'.

    // ACCESSORS
    int allocatorId() const;
        // Return the allocator ID contained in this channel message.

    int channelId() const;
        // Return the channel ID contained in this channel message.

    ChannelState channelState() const;
        // Return the state of this channel message.

    ChannelState event() const;
        // Return the state of this channel message.
        //
        // DEPRECATED  Use 'channelState' instead.
};

// FREE OPERATORS
bool operator==(const btemt_ChannelMsg& lhs,
                const btemt_ChannelMsg& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' channel messages have the
    // same value, and 'false' otherwise.  Two channel messages have the same
    // value if they have the same ID and same allocator ID, and describe the
    // same state.

bsl::ostream& operator<<(bsl::ostream&           stream,
                         const btemt_ChannelMsg& message);
    // Print out the contents of the specified channel 'message' to the
    // specified 'stream' in a human-readable format and return the modifiable
    // 'stream'.

                           // ===================
                           // class btemt_DataMsg
                           // ===================

class btemt_DataMsg {
    // This class provides data associated with a given channel.  The data
    // segment can be passed as a managed or unmanaged pointer to a
    // 'bcema_PooledBufferChain'.  In either case, this class shares management
    // of the data segment.

    // TYPES
    typedef bcema_SharedPtr<bcema_PooledBufferChain> Handle;

    // PRIVATE INSTANCE DATA
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_DataMsg();
        // Create an empty data message.

    btemt_DataMsg(bcema_PooledBufferChain                *chain,
                  bcema_Deleter<bcema_PooledBufferChain> *deleter,
                  int                                     channelId,
                  bslma_Allocator                        *basicAllocator = 0);
        // Create a data message containing the specified 'chain' of bytes and
        // associated with the channel having the specified 'channelId'.  This
        // message will assume ownership of 'chain' and will destroy it using
        // the specified 'deleter' when appropriate.  Optionally specify a
        // 'basicAllocator' used to supply memory for this message.  If
        // 'basicAllocator' is 0, the currently-installed default allocator is
        // used.

    btemt_DataMsg(const bcema_SharedPtr<bcema_PooledBufferChain>&
                                                             dataPtr,
                  int                                        channelId,
                  bslma_Allocator                           *basicAllocator = 0
                 );
        // Create a data message sharing management of the specified 'dataPtr'
        // and associated with the channel having the specified 'channelId'.
        // Note that no allocation is needed and the specified 'basicAllocator'
        // is not used.

    btemt_DataMsg(const btemt_DataMsg& original);
        // Create a data message referring to the same channel and data
        // segment, if any, as the specified 'original' message.

    ~btemt_DataMsg();

    // MANIPULATORS
    btemt_DataMsg& operator=(const btemt_DataMsg& rhs);
        // Assign to this data message the channel ID of, and share management
        // of the data segment referred to by, the specified 'rhs' message.

    void setChannelId(int channelId);
        // Set the channel ID of this data message to the specified
        // 'channelId'.

    void setData(bcema_PooledBufferChain                *chain,
                 bcema_Deleter<bcema_PooledBufferChain> *deleter,
                 bslma_Allocator                        *basicAllocator = 0);
        // Set the data segment for this data message to the specified 'chain'
        // whose lifetime is managed by the specified 'deleter', and release
        // from management by this message any data segment currently managed
        // by this message, if any.

    void setSharedData(
                      const bcema_SharedPtr<bcema_PooledBufferChain>& dataPtr);
        // Set the data content of this data message to the specified
        // 'dataPtr' and share the management of 'dataPtr'.  Also release
        // from management by this message any data segment currently managed
        // by this message, if any.

    void setUserDataField1(int value);
        // Set the user data field 1 of this data message to the specified
        // 'value'.

    void setUserDataField2(int value);
        // Set the user data field 1 of this data message to the specified
        // 'value'.

    // ACCESSORS
    int channelId() const;
        // Return the channel ID of this data message.

    bcema_PooledBufferChain *data() const;
        // Return the data content of this data message represented as a
        // chain of buffers.

    bcema_SharedPtr<bcema_PooledBufferChain>& sharedData() const;
        // Return the data content of this data message represented as a
        // shared pointer to a chain of buffers.

    int userDataField1() const;
        // Return the value in the user data field 1 of this data message.

    int userDataField2() const;
        // Return the value in the user data field 2 of this data message.
};

                          // ===================
                          // class btemt_PoolMsg
                          // ===================

class btemt_PoolMsg {
    // This class provides information about a single event on a channel pool.
    // A pool state and a source ID (e.g., timeout on a certain acceptor) are
    // provided.

  public:
    // ENUMERATIONS
    enum PoolState {
        BTEMT_ACCEPT_TIMEOUT = 0,           // timed out accepting a connection
        BTEMT_ERROR_ACCEPTING,              // error accepting a connection
        BTEMT_ERROR_CONNECTING,             // error connecting to the peer
        BTEMT_CHANNEL_LIMIT,                // channel limit reached
        BTEMT_CAPACITY_LIMIT,               // capacity limit reached
        BTEMT_ERROR_BINDING_CLIENT_ADDR,    // error binding client address
        BTEMT_ERROR_SETTING_OPTIONS,        // error setting socket options
        BTEMT_EVENT_MANAGER_LIMIT           // event manager limit reached

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ACCEPT_TIMEOUT   = BTEMT_ACCEPT_TIMEOUT
      , ERROR_ACCEPTING  = BTEMT_ERROR_ACCEPTING
      , ERROR_CONNECTING = BTEMT_ERROR_CONNECTING
      , CHANNEL_LIMIT    = BTEMT_CHANNEL_LIMIT
      , CAPACITY_LIMIT   = BTEMT_CAPACITY_LIMIT
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
    enum {
        BTEMT_INVALID_ID = -1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INVALID_ID = BTEMT_INVALID_ID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // PRIVATE INSTANCE DATA
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_PoolMsg();
        // Create a pool message having 'BTEMT_INVALID_ID' for the source ID
        // and 'BTEMT_ACCEPT_TIMEOUT' for the event.

    btemt_PoolMsg(PoolState state, int sourceId);
        // Create a pool message having the specified 'state' and the
        // specified source ID.

    btemt_PoolMsg(const btemt_PoolMsg& original);
        // Create a pool message initialized with the value of the specified
        // 'original' message.

    ~btemt_PoolMsg();
        // Destroy this pool message.

    // MANIPULATORS
    btemt_PoolMsg& operator=(const btemt_PoolMsg& rhs);
        // Assign to this pool message the value of the specified 'rhs' message
        // and return a reference to this modifiable message.

    void setPoolState(PoolState state);
        // Set the state of this pool message to the specified 'state'.

    void setSourceId(int id);
        // Set the source ID contained in this pool message to the specified
        // 'id'.

    void setState(PoolState state);
        // Set the state of this pool message to the specified 'state'.
        //
        // DEPRECATED  Use 'setPoolState' instead.

    // ACCESSORS
    PoolState event() const;
        // Return the state described by this pool message.
        //
        // DEPRECATED  Use 'poolState' instead.

    PoolState poolState() const;
        // Return the state described by this pool message.

    int sourceId() const;
        // Return the source ID for this pool message.
};

// FREE OPERATORS
bool operator==(const btemt_PoolMsg& lhs,
                const btemt_PoolMsg& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' pool messages have the
    // same value, and 'false' otherwise.  Two pool messages have the same
    // value if they have the same source ID, and describe the
    // same event.

bsl::ostream& operator<<(bsl::ostream&        stream,
                         const btemt_PoolMsg& message);
    // Print out the contents of the specified pool 'message' to the specified
    // 'stream' in a human-readable format and return the modifiable 'stream'.

                          // ====================
                          // class btemt_TimerMsg
                          // ====================

class btemt_TimerMsg {
    // This class provides information about a timer event.  A user-assigned
    // unique timer ID is provided.

  public:
    // ENUMERATIONS
    enum {
        BTEMT_INVALID_ID = -1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INVALID_ID = BTEMT_INVALID_ID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_TimerMsg();
        // Create a timer message having 'BTEMT_INVALID_ID' for the timer ID.

    btemt_TimerMsg(int timerId);
        // Create a timer message having the specified 'timerId'.

    btemt_TimerMsg(const btemt_TimerMsg& original);
        // Create a timer message initialized with the value of the
        // specified 'original' message.

    ~btemt_TimerMsg();
        // Destroy this timer message.

    // MANIPULATORS
    btemt_TimerMsg& operator=(const btemt_TimerMsg& rhs);
        // Assign to this timer message the value of the specified 'rhs'
        // message and return the reference to this modifiable message.

    void setTimerId(int timerId);
        // Set the timer ID of this timer message to the specified 'timerId'.

    // ACCESSORS
    int timerId() const;
        // Return the timer ID associated with this timer message.
};

// FREE OPERATORS
bool operator==(const btemt_TimerMsg& lhs, const btemt_TimerMsg& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timer messages have the
    // same value, and 'false' otherwise.  Two timer messages have the same
    // value if they have the same timer ID.

bsl::ostream& operator<<(bsl::ostream& stream, const btemt_TimerMsg& msg);
    // Print out the contents of the specified 'msg' to the specified 'stream'
    // in a human-readable format and return the modifiable 'stream'.

                          // ===================
                          // class btemt_UserMsg
                          // ===================

class btemt_UserMsg {
    // This class provides a place-holder for user data.  A 'btemt_UserMsg' is
    // capable of holding an integer value, as well as one of two types of
    // pointer data: an opaque data stored as an (unmanaged) pointer of type
    // 'void*', or a (managed) pointer to a 'bcema_PooledBufferChain'.
    //
    // TBD  Add support for Blobs here as well...

    // PRIVATE TYPES
    typedef bcema_SharedPtr<bcema_PooledBufferChain> Handle;

  public:
    // ENUMERATIONS
    enum {
        BTEMT_INVALID_INT_DATA     = -1
      , BTEMT_INVALID_VOID_DATA    =  0
      , BTEMT_INVALID_MESSAGE_TYPE = -1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INVALID_INT_DATA     = BTEMT_INVALID_INT_DATA
      , INVALID_VOID_DATA    = BTEMT_INVALID_VOID_DATA
      , INVALID_MESSAGE_TYPE = BTEMT_INVALID_MESSAGE_TYPE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
  private:
    enum {
        BTEMT_OPAQUE_VALUE = 0  // the 'void*' data member is not managed
      , BTEMT_MANAGED      = 1  // the 'void*' data member is managed by the
                                // counted handle
    };
    mutable btemt_MessageImpl d_impl;

  public:
    // CREATORS
    btemt_UserMsg();
        // Create a user message of 'INVALID_MESSAGE_TYPE' type that has
        // 'INVALID_INT_DATA' and '(void*)INVALID_VOID_DATA' in the
        // user data fields.

    btemt_UserMsg(const btemt_UserMsg& original);
        // Create a user message initialized with the integer and pointer
        // values of the specified 'original' message.  If 'original' holds a
        // managed pointer value, then this message will manage that pointer
        // value as well.

    ~btemt_UserMsg();
        // Destroy this user message.

    // MANIPULATORS
    btemt_UserMsg& operator=(const btemt_UserMsg& rhs);
        // Assign to this user message the value of the specified 'rhs' message
        // and return the reference to this modifiable message.

    void setIntData(int value);
        // Store the specified integer 'value' into this user message.

    void setVoidPtrData(void *value);
        // Store the specified (unmanaged) pointer 'value' into this user
        // message.  If this message previously contained managed data, that
        // data is released from management by this message.

    void setCountedData(
                   bcema_PooledBufferChain                *chain,
                   bcema_Deleter<bcema_PooledBufferChain> *deleter,
                   bslma_Allocator                        *basicAllocator = 0);
        // DEPRECATED  Use 'setManagedData' instead.

    void setManagedData(
                   bcema_PooledBufferChain                *chain,
                   bcema_Deleter<bcema_PooledBufferChain> *deleter,
                   bslma_Allocator                        *basicAllocator = 0);
        // Store the specified 'chain' pointer value allocated from the
        // specified 'factory' into this user message, release from management
        // any data currently managed by this message, and start managing
        // 'chain' using the specified 'deleter' to destroy 'chain' when
        // appropriate.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently-installed
        // default allocator is used.

    void setType(int value);
        // Set the type of this user message to the specified 'value'.

    // ACCESSORS
    int intData() const;
        // Return the integer data value stored in this user message.

    bool isCounted() const;
        // DEPRECATED  Use 'isManaged' instead.

    bool isManaged() const;
        // Return 'true' if this user message contains managed data and 'false'
        // otherwise.

    int type() const;
        // Return the type of this message.

    void *voidPtrData() const;
        // Return the 'void*' data value stored in this message.  The behavior
        // is undefined unless this message contains an opaque (i.e., not
        // managed) data pointer as reported by the 'isManaged' method.

    bcema_PooledBufferChain *countedData() const;
        // Return the address of the managed data stored in this user message.
        // The behavior is undefined unless this message contains a managed
        // data pointer as reported by the 'isManaged' method.
        //
        // DEPRECATED  Use 'managedData' instead.

    bcema_SharedPtr<bcema_PooledBufferChain> managedData() const;
        // Return a managed pointer pointing to the same pointer data stored in
        // this user message.  The behavior is undefined unless this message
        // contains a managed data as reported by the 'isManaged' method.
};

                             // ===================
                             // class btemt_Message
                             // ===================

class btemt_Message {
    // This class provides a convenience container for holding any of the five
    // specific-message types.  Note that the preferred usage of this container
    // is to initialize a 'btemt_Message' object with a fully-constructed
    // specific-message object (see the usage example in the component-level
    // documentation).

  public:
    enum MessageType {
        BTEMT_DATA            = 0
      , BTEMT_CHANNEL_STATE
      , BTEMT_TIMER
      , BTEMT_POOL_STATE
      , BTEMT_USER_DATA
      , BTEMT_BLOB
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DATA          = BTEMT_DATA
      , CHANNEL_STATE = BTEMT_CHANNEL_STATE
      , TIMER         = BTEMT_TIMER
      , POOL_STATE    = BTEMT_POOL_STATE
      , USER_DATA     = BTEMT_USER_DATA
      , BLOB          = BTEMT_BLOB
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    mutable btemt_MessageImpl d_data;  // data holder
    MessageType               d_type;  // message type

  public:
    // CREATORS
    btemt_Message(MessageType type);
        // Create a message of the specified message 'type' as if by calling
        // the default constructor of that message type.

    btemt_Message(const btemt_BlobMsg& message);
        // Create a blob message having the value of the specified 'message'.

    btemt_Message(const btemt_ChannelMsg& message);
        // Create a channel state message having the value of the specified
        // 'message'.

    btemt_Message(const btemt_DataMsg& message);
        // Create a data message having the value of the specified 'message'.

    btemt_Message(const btemt_PoolMsg& message);
        // Create a pool message having the value of the specified 'message'.

    btemt_Message(const btemt_TimerMsg& message);
        // Create a timer message having the value of the specified 'message'.

    btemt_Message(const btemt_UserMsg& message);
        // Create a user message having the value of the specified 'message'.

    btemt_Message(const btemt_Message& original);
        // Create a messages initialized with the type and value of the
        // specified 'original' message.

    ~btemt_Message();
        // Destroy this message.

    // MANIPULATORS
    btemt_Message& operator=(const btemt_Message& rhs);
        // Assign to this message the type and value of the specified 'rhs'
        // message and return a reference to this modifiable message.

    btemt_BlobMsg& blobMsg();
        // Return a reference to the modifiable blob message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'BLOB').

    btemt_ChannelMsg& channelMsg();
        // Return a reference to the modifiable channel message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns
        // 'BTEMT_CHANNEL_STATE').

    btemt_DataMsg& dataMsg();
        // Return a reference to the modifiable data message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'DATA').

    btemt_PoolMsg& poolMsg();
        // Return a reference to the modifiable pool message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'POOL_STATE').

    btemt_TimerMsg& timerMsg();
        // Return a reference to the modifiable timer message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'TIMER').

    btemt_UserMsg& userMsg();
        // Return a reference to the modifiable user message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'USER_DATA').

    // ACCESSORS
    MessageType type() const;
        // Return the type of this message.

    const btemt_BlobMsg& blobMsg() const;
        // Return a reference to the modifiable blob message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'BLOB').

    const btemt_ChannelMsg& channelMsg() const;
        // Return a reference to the non-modifiable channel message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns
        // 'BTEMT_CHANNEL_STATE').

    const btemt_DataMsg& dataMsg() const;
        // Return a reference to the non-modifiable data message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'DATA').

    const btemt_PoolMsg& poolMsg() const;
        // Return a reference to the non-modifiable pool message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'POOL_STATE').

    const btemt_TimerMsg& timerMsg() const;
        // Return a reference to the non-modifiable timer message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'TIMER').

    const btemt_UserMsg& userMsg() const;
        // Return a reference to the non-modifiable user message contained
        // in this message.  The behavior is undefined unless this message
        // is of appropriate type (i.e., 'type()' returns 'USER_DATA').
};

struct btemt_MessageUtil {
    static void assignData(bcema_Blob           *blob,
                           const btemt_DataMsg&  dataMsg,
                           int                   numBytes);
        // Assign the specified initial 'numBytes' of the specified 'dataMsg'
        // to the specified 'blob' resetting its length to 'numBytes'.

    static void assignData(btemt_DataMsg                  *dataMsg,
                           const bcema_Blob&               blob,
                           int                             numBytes,
                           bcema_PooledBufferChainFactory *factory,
                           bslma_Allocator                *spAllocator);
        // Assign the specified initial 'numBytes' of the specified 'blob'
        // to the specified 'dataMsg', using the specified 'factory' to 
        // allocate buffers in the pooled buffer chain and using the specified
        // 'spAllocator' to allocate the shared pointers.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                             // -------------------
                             // class btemt_BlobMsg
                             // -------------------

// CREATORS
inline
btemt_BlobMsg::btemt_BlobMsg()
{
    d_impl.d_channelId = -1;
    new (d_impl.d_data.d_arena) Handle();
}

inline
btemt_BlobMsg::btemt_BlobMsg(const bcema_SharedPtr<bcema_Blob>&  dataPtr,
                             int                                 channelId)
{
    new (d_impl.d_data.d_arena) Handle(dataPtr);
    d_impl.d_channelId = channelId;
}

inline
btemt_BlobMsg::~btemt_BlobMsg()
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    h->~Handle();
}

// MANIPULATORS
inline
void btemt_BlobMsg::setData(bcema_Blob                *blob,
                            bslma_Allocator           *basicAllocator)
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    h->load(blob, basicAllocator);
}

inline
void btemt_BlobMsg::setSharedData(const bcema_SharedPtr<bcema_Blob>& dataPtr)
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    *h = dataPtr;
}

inline
void btemt_BlobMsg::setChannelId(int channelId)
{
    d_impl.d_channelId = channelId;
}

inline
void btemt_BlobMsg::setUserDataField1(int value)
{
    d_impl.d_dataLength = value;
}

inline
void btemt_BlobMsg::setUserDataField2(int value)
{
    d_impl.d_bufferLength = value;
}

// ACCESSORS
inline
int btemt_BlobMsg::channelId() const
{
    return d_impl.d_channelId;
}

inline
bcema_Blob *btemt_BlobMsg::data() const
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return h->ptr();
}

inline
bcema_SharedPtr<bcema_Blob>& btemt_BlobMsg::sharedData() const
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return *h;
}

inline
int btemt_BlobMsg::userDataField1() const
{
    return d_impl.d_dataLength;
}

inline
int btemt_BlobMsg::userDataField2() const
{
    return d_impl.d_bufferLength;
}

                           // ----------------------
                           // class btemt_ChannelMsg
                           // ----------------------

// CREATORS
inline
btemt_ChannelMsg::btemt_ChannelMsg()
{
    d_impl.d_channelId = BTEMT_INVALID_ID;
    d_impl.d_bufferLength = BTEMT_INVALID_ID;
    d_impl.d_dataLength = (int) BTEMT_CHANNEL_DOWN;
}

inline
btemt_ChannelMsg::btemt_ChannelMsg(ChannelState state, int channelId,
                                   int allocatorId)
{
    d_impl.d_channelId = channelId;
    d_impl.d_bufferLength = allocatorId;
    d_impl.d_dataLength = (int) state;
}

inline
btemt_ChannelMsg::btemt_ChannelMsg(const btemt_ChannelMsg& original)
: d_impl(original.d_impl)
{
}

inline
btemt_ChannelMsg::~btemt_ChannelMsg()
{
}

// MANIPULATORS
inline
btemt_ChannelMsg& btemt_ChannelMsg::operator=(const btemt_ChannelMsg& rhs)
{
    if (this != &rhs) {
        d_impl = rhs.d_impl;
    }
    return *this;
}
inline
void btemt_ChannelMsg::setChannelId(int id)
{
    d_impl.d_channelId = id;
}

inline
void btemt_ChannelMsg::setChannelState(btemt_ChannelMsg::ChannelState event)
{
    d_impl.d_dataLength = int(event);
}

inline
void btemt_ChannelMsg::setAllocatorId(int id)
{
    d_impl.d_bufferLength = id;
}

// ACCESSORS
inline
int btemt_ChannelMsg::channelId() const
{
    return d_impl.d_channelId;
}

inline
btemt_ChannelMsg::ChannelState btemt_ChannelMsg::channelState() const
{
    return (ChannelState) d_impl.d_dataLength;
}

inline
btemt_ChannelMsg::ChannelState btemt_ChannelMsg::event() const
{
    return channelState();
}

inline
int btemt_ChannelMsg::allocatorId() const
{
    return d_impl.d_bufferLength;
}

                             // -------------------
                             // class btemt_DataMsg
                             // -------------------

// CREATORS
inline
btemt_DataMsg::btemt_DataMsg()
{
    d_impl.d_channelId = -1;
    new (d_impl.d_data.d_arena) Handle();
}

inline
btemt_DataMsg::btemt_DataMsg(
        bcema_PooledBufferChain                *chain,
        bcema_Deleter<bcema_PooledBufferChain> *deleter,
        int                                     channelId,
        bslma_Allocator                        *basicAllocator)
{
    new (d_impl.d_data.d_arena) Handle(chain, deleter, basicAllocator);
    d_impl.d_channelId = channelId;
}

inline
btemt_DataMsg::btemt_DataMsg(
        const bcema_SharedPtr<bcema_PooledBufferChain>&  dataPtr,
        int                                              channelId,
        bslma_Allocator                                 *)
{
    new (d_impl.d_data.d_arena) Handle(dataPtr);
    d_impl.d_channelId = channelId;
}

inline
btemt_DataMsg::~btemt_DataMsg()
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    h->~Handle();
}

// MANIPULATORS
inline
void btemt_DataMsg::setChannelId(int channelId)
{
    d_impl.d_channelId = channelId;
}

inline
void btemt_DataMsg::setData(
        bcema_PooledBufferChain                *chain,
        bcema_Deleter<bcema_PooledBufferChain> *deleter,
        bslma_Allocator                        *basicAllocator)
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    h->load(chain, deleter, basicAllocator);
}

inline
void btemt_DataMsg::setSharedData(
        const bcema_SharedPtr<bcema_PooledBufferChain>& dataPtr)
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    *h = dataPtr;
}

inline
void btemt_DataMsg::setUserDataField1(int value)
{
    d_impl.d_dataLength = value;
}

inline
void btemt_DataMsg::setUserDataField2(int value)
{
    d_impl.d_bufferLength = value;
}

// ACCESSORS
inline
int btemt_DataMsg::channelId() const
{
    return d_impl.d_channelId;
}

inline
bcema_PooledBufferChain *btemt_DataMsg::data() const
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return h->ptr();
}

inline
bcema_SharedPtr<bcema_PooledBufferChain>& btemt_DataMsg::sharedData() const
{
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return *h;
}

inline
int btemt_DataMsg::userDataField1() const
{
    return d_impl.d_dataLength;
}

inline
int btemt_DataMsg::userDataField2() const
{
    return d_impl.d_bufferLength;
}

                             // -------------------
                             // class btemt_PoolMsg
                             // -------------------

// CREATORS
inline
btemt_PoolMsg::btemt_PoolMsg()
{
    d_impl.d_channelId = BTEMT_INVALID_ID;
    d_impl.d_dataLength = 0;
}

inline
btemt_PoolMsg::btemt_PoolMsg(PoolState state, int sourceId)
{
    d_impl.d_channelId = sourceId;
    d_impl.d_dataLength = (int) state;
}

inline
btemt_PoolMsg::btemt_PoolMsg(const btemt_PoolMsg& original)
: d_impl(original.d_impl)
{
}

inline
btemt_PoolMsg::~btemt_PoolMsg()
{
}

// MANIPULATORS
inline
btemt_PoolMsg& btemt_PoolMsg::operator=(const btemt_PoolMsg& rhs)
{
    if (this != &rhs) {
        d_impl = rhs.d_impl;
    }
    return *this;
}

inline
void btemt_PoolMsg::setPoolState(btemt_PoolMsg::PoolState state)
{
    d_impl.d_dataLength = int(state);
}

inline
void btemt_PoolMsg::setSourceId(int id)
{
    d_impl.d_channelId = id;
}

inline
void btemt_PoolMsg::setState(btemt_PoolMsg::PoolState state)
{
    setPoolState(state);
}

// ACCESSORS
inline
int btemt_PoolMsg::sourceId() const
{
    return d_impl.d_channelId;
}

inline
btemt_PoolMsg::PoolState btemt_PoolMsg::event() const
{
    return poolState();
}

inline
btemt_PoolMsg::PoolState btemt_PoolMsg::poolState() const
{
    return (PoolState) d_impl.d_dataLength;
}

// FREE OPERATORS
inline
bool operator==(const btemt_PoolMsg& lhs,
                const btemt_PoolMsg& rhs)
{
    return lhs.sourceId() == rhs.sourceId()
        && lhs.poolState() == rhs.poolState();
}

                            // --------------------
                            // class btemt_TimerMsg
                            // --------------------

// CREATORS
inline
btemt_TimerMsg::btemt_TimerMsg()
{
    d_impl.d_channelId = BTEMT_INVALID_ID;
}

inline
btemt_TimerMsg::btemt_TimerMsg(int timerId)
{
    d_impl.d_channelId = timerId;
}

inline
btemt_TimerMsg::btemt_TimerMsg(const btemt_TimerMsg& original)
: d_impl(original.d_impl)
{
}

inline
btemt_TimerMsg::~btemt_TimerMsg()
{
}

// MANIPULATORS
inline
btemt_TimerMsg& btemt_TimerMsg::operator=(const btemt_TimerMsg& rhs)
{
    if (this != &rhs) {
        d_impl = rhs.d_impl;
    }
    return *this;
}

inline
void btemt_TimerMsg::setTimerId(int id)
{
    d_impl.d_channelId = id;
}

// ACCESSORS
inline
int btemt_TimerMsg::timerId() const
{
    return d_impl.d_channelId;
}

// FREE OPERATORS
inline
bool operator==(const btemt_TimerMsg& lhs,
                const btemt_TimerMsg& rhs)
{
    return lhs.timerId() == rhs.timerId();
}

                             // -------------------
                             // class btemt_UserMsg
                             // -------------------

// MANIPULATORS
inline
void btemt_UserMsg::
         setCountedData(bcema_PooledBufferChain                *chain,
                        bcema_Deleter<bcema_PooledBufferChain> *deleter,
                        bslma_Allocator                        *basicAllocator)
{
    setManagedData(chain, deleter, basicAllocator);
}

inline
void btemt_UserMsg::setType(int value)
{
    d_impl.d_channelId = value;
}

inline
void btemt_UserMsg::setIntData(int value)
{
    d_impl.d_bufferLength = value;

}

// ACCESSORS
inline
bcema_PooledBufferChain *btemt_UserMsg::countedData() const
{
    // Valid only if message contains MANAGED data.
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return h->ptr();
}

inline
bcema_SharedPtr<bcema_PooledBufferChain> btemt_UserMsg::managedData() const
{
    // Valid only if message contains MANAGED data.
    Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
    return *h;
}

inline
int btemt_UserMsg::type() const
{
    return d_impl.d_channelId;

}

inline
int btemt_UserMsg::intData() const
{
    return d_impl.d_bufferLength;
}

inline
bool btemt_UserMsg::isCounted() const
{
    return isManaged();
}

inline
bool btemt_UserMsg::isManaged() const
{
    return BTEMT_MANAGED == d_impl.d_dataLength;
}

inline
void *btemt_UserMsg::voidPtrData() const
{
    // Valid only if message contains OPAQUE_VALUE data.
    return d_impl.d_data.d_opaque;
}

                             // -------------------
                             // class btemt_Message
                             // -------------------

// CREATORS
inline
btemt_Message::btemt_Message(const btemt_BlobMsg& message)
: d_type(BTEMT_BLOB)
{
    new ((void*)&d_data) btemt_BlobMsg(message);
}

inline
btemt_Message::btemt_Message(const btemt_ChannelMsg& message)
: d_type(BTEMT_CHANNEL_STATE)
{
    new ((void*)&d_data) btemt_ChannelMsg(message);
}

inline
btemt_Message::btemt_Message(const btemt_DataMsg& message)
: d_type(BTEMT_DATA)
{
    new ((void*)&d_data) btemt_DataMsg(message);
}

inline
btemt_Message::btemt_Message(const btemt_PoolMsg& message)
: d_type(BTEMT_POOL_STATE)

{
    new ((void*)&d_data) btemt_PoolMsg(message);
}

inline
btemt_Message::btemt_Message(const btemt_TimerMsg& message)
: d_type(BTEMT_TIMER)
{
    new ((void*)&d_data) btemt_TimerMsg(message);
}

inline
btemt_Message::btemt_Message(const btemt_UserMsg& message)
: d_type(BTEMT_USER_DATA)
{
    new ((void*)&d_data) btemt_UserMsg(message);
}

// MANIPULATORS
inline
btemt_BlobMsg& btemt_Message::blobMsg()
{
    return *(btemt_BlobMsg*)&d_data;
}

inline
btemt_ChannelMsg&  btemt_Message::channelMsg()
{
    return *(btemt_ChannelMsg*)&d_data;
}

inline
btemt_DataMsg& btemt_Message::dataMsg()
{
    return *(btemt_DataMsg*)&d_data;
}

inline
btemt_PoolMsg& btemt_Message::poolMsg()
{
    return *(btemt_PoolMsg*)&d_data;
}

inline
btemt_TimerMsg& btemt_Message::timerMsg()
{
    return *(btemt_TimerMsg*)&d_data;
}

inline
btemt_UserMsg& btemt_Message::userMsg()
{
    return *(btemt_UserMsg*)&d_data;
}

// ACCESSORS
inline
btemt_Message::MessageType btemt_Message::type() const
{
    return d_type;
}

inline
const btemt_BlobMsg& btemt_Message::blobMsg() const
{
    return *(btemt_BlobMsg*)&d_data;
}

inline
const btemt_ChannelMsg&  btemt_Message::channelMsg() const
{
    return *(btemt_ChannelMsg*)&d_data;
}

inline
const btemt_DataMsg& btemt_Message::dataMsg() const
{
    return *(btemt_DataMsg*)&d_data;
}

inline
const btemt_PoolMsg& btemt_Message::poolMsg() const
{
    return *(btemt_PoolMsg*)&d_data;
}

inline
const btemt_TimerMsg& btemt_Message::timerMsg() const
{
    return *(btemt_TimerMsg*)&d_data;
}

inline
const btemt_UserMsg& btemt_Message::userMsg() const
{
    return *(btemt_UserMsg*)&d_data;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
