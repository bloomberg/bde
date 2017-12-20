// btlmt_connectoptions.h                                             -*-C++-*-
#ifndef INCLUDED_BTLMT_CONNECTOPTIONS
#define INCLUDED_BTLMT_CONNECTOPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide attribute class characterizing client 'connect' options.
//
//@CLASSES:
//  btlmt::ConnectOptions: attributes characterizing 'connect' options
//
//@SEE_ALSO: btlmt_channelpool
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'btlmt::ConnectOptions', that is used to
// characterize options for connecting to servers.
//
///Attributes
///----------
//..
//  Name                      Type                                  Constraints
//  ------                    ----                                  -----------
//  serverEndpoint            Variant2<EndPoint, IPv4Address>           none
//
//  numAttempts               int                                       > 0
//
//  timeout                   bsls::TimeInterval                        none
//
//  enableRead                bool                                      none
//
//  allowHalfOpenConnections  bool                                      none
//
//  resolutionMode            btlmt::ResolutionMode::Enum               none
//
//  socketOptions             NullableValue<btlso::SocketOptions>       none
//
//  localAddress              NullableValue<btlso::IPv4Address>         none
//
//  socketPtr                 ManagedPtr<StreamSocket<IPv4Address> > *  none
//..
//: o 'serverEndpoint':
//:   o Type: bdlb::Variant2<btlso::EndPoint, btlso::IPv4Address>
//:   o Default: bdlb::Variant2<btlso::EndPoint, btlso::IPv4Address>()
//:   o Constraints: none
//:   o Description: The server address to connect to.
//:
//: o 'numAttempts':
//:   o Type: int
//:   o Default: 1
//:   o Constraints: > 0
//:   o Description: The maximum number of connection attempts.
//:
//: o 'timeout':
//:   o Type: bsls::TimeInterval
//:   o Default: bsls::TimeInterval()
//:   o Constraints: none
//:   o Description: The time duration to wait between 'connection' attempts.
//:
//: o 'enableRead':
//:   o Type: bool
//:   o Default: true
//:   o Constraints: none
//:   o Description: A flag specifying if read operations should be enabled for
//:     accepted connections.  A 'true' value for this flag implies that
//:     incoming data on new connections will be automatically monitored and a
//:     'false' value implies that incoming data will be ignored.
//:
//: o 'allowHalfOpenConnections':
//:   o Type: bool
//:   o Default: false
//:   o Constraints: none
//:   o Description: A flag specifying if half-open connections are allowed.
//:     A connection is half-open if only one of its receive or send end (but
//:     not both) is active at a time.  If this flag is 'true' and a connection
//:     closes its send part but keeps its receive part open, then outgoing
//:     data will no longer be sent but incoming data will continue to be read.
//:     Similarly, if this flag is 'true' and a connection closes its receive
//:     part but keeps its send part open, then any queued data will continue
//:     to be sent but no new data is read.  If this flag is 'false' then
//:     closure of any end of the connection will result in both ends of the
//:     connection being closed.
//:
//: o 'resolutionMode':
//:   o Type: btlmt::ResolutionMode::Enum
//:   o Default: btlmt::ResolutionMode::e_RESOLVE_ONCE
//:   o Constraints: none
//:   o Description: A flag specifying whether IP addresses provided as
//:     domain names should be resolved once or on each 'connect' attempt.
//:
//: o 'socketOptions':
//:   o Type: bdlb::NullableValue<btlso::SocketOptions>
//:   o Default: null
//:   o Constraints: none
//:   o Description: Socket options to use for the connecting socket.
//:
//: o 'localAddress':
//:   o Type: bdlb::NullableValue<btlso::IPv4Address>
//:   o Default: null
//:   o Constraints: none
//:   o Description: The local address to bind this connection to.
//:
//: o 'socketPtr':
//:   o Type: bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > *
//:   o Default: 0
//:   o Constraints: none
//:   o Description: A managed pointer to a fully initialized stream socket
//:     that represents the local socket to use for the connection.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Establishing a connection to a server
/// - - - - - - - - - - - - - - - - - - - - - - - -
// This component allows specifying configuration options for creating sockets
// that connect to servers.  Consider that a client wants to establish a
// connection to a server on a remote machine.  The following example will show
// how to provide configuration options for doing so.
//
// We will assume that we have a 'connect' method that accepts a
// 'btlmt::ConnectOptions' object as an argument, connects to the server
// address in that object, and provides the socket handle of that socket.  The
// signature of that function is provided below but for brevity the
// implementation is elided (note that in practice clients would likely use a
// higher level component like 'btlmt_sessionpool' or 'btlmt_channelpool' for
// connecting to remote servers):
//..
//  int connect(btlso::SocketHandle::Handle  *handle,
//              const btlmt::ConnectOptions&  options)
//      // Connect to the server based on the specified connect 'options'.  On
//      // success, load into the specified 'handle' the handle for the socket
//      // created for the newly established connection.  Return 0 on success
//      // and a non-zero value otherewise.
//  {
//      // . . .
//
//      return 0;
//  }
//..
// First, we will default-construct the connect options object:
//..
//  btlmt::ConnectOptions options;
//  assert(ConnectOptions::ServerEndpoint() == options.serverEndpoint());
//  assert(1                                == options.numAttempts());
//  assert(bsls::TimeInterval()             == options.timeout());
//  assert(true                             == options.enableRead());
//  assert(ResolutionMode::e_RESOLVE_ONCE   == options.resolutionMode());
//  assert(false                        == options.allowHalfOpenConnections());
//  assert(options.socketOptions().isNull());
//  assert(options.localAddress().isNull());
//  assert(!options.socketPtr());
//..
// Next, we specify the server address to connect to:
//..
//  btlso::Endpoint address("www.bloomberg.com", 80);
//
//  options.setServerEndpoint(address);
//  assert(address == options.serverEndpoint().the<btlso::Endpoint>());
//..
// Then, we will specify the number of attempts that the connecting method
// make before reporting an unsuccessful connection:
//..
//  const int NUM_ATTEMPTS = 10;
//  options.setNumAttempts(NUM_ATTEMPTS);
//  assert(NUM_ATTEMPTS == options.numAttempts());
//..
// Next, the time to wait for a connect attempt to succeed is specified.  If a
// connect attempt does not succeed within that time frame the 'connect'
// method can invoke the user callback with a timeout status.
//..
//  const bsls::TimeInterval TIMEOUT(2);
//  options.setTimeout(TIMEOUT);
//  assert(TIMEOUT == options.timeout().value());
//..
// Then, we will populate a flag that specifies whether incoming connections
// can be half-open.  A half-open connection has only its read-end or write-end
// open.  For a connection the server may decide to close the read end while
// continuing to enqueue write data.
//..
//  const bool ALLOW_HALF_OPEN_CONNECTIONS = true;
//  options.setAllowHalfOpenConnections(ALLOW_HALF_OPEN_CONNECTIONS);
//  assert(ALLOW_HALF_OPEN_CONNECTIONS == options.allowHalfOpenConnections());
//..
// Now, we specify certain socket options for the connecting socket such as
// allowing address reuse and disabling Nagle's data coalescing algorithm:
//..
//  btlso::SocketOptions socketOptions;
//  socketOptions.setReuseAddress(true);
//  socketOptions.setTcpNoDelay(true);
//  options.setSocketOptions(socketOptions);
//  assert(socketOptions == options.socketOptions());
//..
// Finally, we call the 'listen' method with the constructed 'options':
//..
//  btlso::SocketHandle::Handle handle;
//  const int rc = connect(&handle, options);
//  assert(0 == rc);
//
//  // Start reading and writing to the connection using 'handle'
//  // . . .
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLMT_RESOLUTIONMODE
#include <btlmt_resolutionmode.h>
#endif

#ifndef INCLUDED_BTLSO_ENDPOINT
#include <btlso_endpoint.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#include <btlso_streamsocket.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETOPTIONS
#include <btlso_socketoptions.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlmt {

                       // ====================
                       // class ConnectOptions
                       // ====================

class ConnectOptions {
    // This simply unconstrained (value-semantic) attribute class characterizes
    // the set of options for connecting to servers.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.  Note that the class invariants are
    // identically the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    typedef bdlb::Variant2<btlso::Endpoint, btlso::IPv4Address> ServerEndpoint;

  private:
    // DATA
    ServerEndpoint               d_serverEndpoint;  // server address to
                                                    // connect to

    int                          d_numAttempts;     // num of connect attempts

    bsls::TimeInterval           d_timeout;         // time to wait between
                                                    // connect attempts

    bool                         d_enableRead;      // enable read operations
                                                    // flag

    bool                         d_allowHalfOpenConnections;
                                                    // allowHalfOpenConnections

    btlmt::ResolutionMode::Enum  d_resolutionMode;  // resolution mode

    bdlb::NullableValue<btlso::SocketOptions>
                                 d_socketOptions;   // socket options

    bdlb::NullableValue<btlso::IPv4Address>
                                 d_localAddress;    // local address to
                                                    // connect from

    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                *d_socketPtr;       // managed ptr

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ConnectOptions, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(ConnectOptions, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit ConnectOptions(bslma::Allocator *basicAllocator = 0);
        // Create a socket options object having none of the option values
        // non-null.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, use the currently installed
        // default allocator.

    ConnectOptions(const ConnectOptions&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a socket options object having the value of the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, use the currently
        // installed default allocator.

    ~ConnectOptions();
        // Destroy this object.

    // MANIPULATORS
    ConnectOptions& operator=(const ConnectOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this socket options object to the default value (i.e., its
        // value upon default construction).

    void setServerEndpoint(const btlso::IPv4Address& value);
    void setServerEndpoint(const btlso::Endpoint& value);
        // Set the 'serverEndpoint' attribute of this object to the specified
        // 'value'.

    void setNumAttempts(int value);
        // Set the 'numAttempts' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 < value'.

    void setTimeout(const bsls::TimeInterval& value);
        // Set the 'timeout' attribute of this object to the specified 'value'.

    void setEnableRead(bool value);
        // Set the 'enableRead' attribute of this object to the specified
        // 'value'.

    void setAllowHalfOpenConnections(bool value);
        // Set the 'allowHalfOpenConnections' attribute of this object to the
        // specified 'value'.

    void setResolutionMode(btlmt::ResolutionMode::Enum value);
        // Set the 'resolutionMode' attribute of this object to the specified
        // 'value'.

    void setSocketOptions(const btlso::SocketOptions& value);
        // Set the 'socketOptions' attribute of this object to the specified
        // 'value'.

    void setLocalAddress(const btlso::IPv4Address& value);
        // Set the 'localAddress' attribute of this object to the specified
        // 'value'.

    void setSocketPtr(
           bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > *value);
        // Set the 'socketPtr' attribute of this object to the specified
        // 'value'.

                                  // Aspects

    void swap(ConnectOptions& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const ServerEndpoint& serverEndpoint() const;
        // Return a reference to the non-modifiable 'serverEndpoint' attribute.

    int numAttempts() const;
        // Return a reference to the non-modifiable 'numAttempts' attribute.

    const bsls::TimeInterval& timeout() const;
        // Return a reference to the non-modifiable 'timeout' attribute.

    bool enableRead() const;
        // Return a reference to the non-modifiable 'enableRead' attribute.

    bool allowHalfOpenConnections() const;
        // Return a reference to the non-modifiable 'AllowHalfOpenConnections'
        // attribute.

    btlmt::ResolutionMode::Enum resolutionMode() const;
        // Return a reference to the non-modifiable 'resolutionMode' attribute.

    const bdlb::NullableValue<btlso::SocketOptions>& socketOptions() const;
        // Return a reference to the non-modifiable 'socketOptions' attribute.

    const bdlb::NullableValue<btlso::IPv4Address>& localAddress() const;
        // Return a reference to the non-modifiable 'localAddress' attribute.

    const bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > *
    socketPtr() const;
        // Return a reference to the non-modifiable 'socketPtr' attribute.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

};

// FREE OPERATORS
bool operator==(const btlmt::ConnectOptions& lhs,
                const btlmt::ConnectOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const btlmt::ConnectOptions& lhs,
                const btlmt::ConnectOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream& stream, const ConnectOptions& object);
    // Format the specified 'object' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
void swap(ConnectOptions& a, ConnectOptions& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // --------------------
                       // class ConnectOptions
                       // --------------------

// CREATORS
inline
ConnectOptions::ConnectOptions(const ConnectOptions&  original,
                               bslma::Allocator      *basicAllocator)
: d_serverEndpoint(original.d_serverEndpoint, basicAllocator)
, d_numAttempts(original.d_numAttempts)
, d_timeout(original.d_timeout)
, d_enableRead(original.d_enableRead)
, d_allowHalfOpenConnections(original.d_allowHalfOpenConnections)
, d_resolutionMode(original.d_resolutionMode)
, d_socketOptions(original.d_socketOptions)
, d_localAddress(original.d_localAddress)
, d_socketPtr(original.d_socketPtr)
{
}

inline
ConnectOptions::~ConnectOptions()
{
}

// MANIPULATORS
inline
void ConnectOptions::setServerEndpoint(const btlso::IPv4Address& value)
{
    d_serverEndpoint = value;
}

inline
void ConnectOptions::setServerEndpoint(const btlso::Endpoint& value)
{
    d_serverEndpoint = value;
}

inline
void ConnectOptions::setNumAttempts(int value)
{
    BSLS_ASSERT_SAFE(0 < value);

    d_numAttempts = value;
}

inline
void ConnectOptions::setTimeout(const bsls::TimeInterval& value)
{
    d_timeout = value;
}

inline
void ConnectOptions::setEnableRead(bool value)
{
    d_enableRead = value;
}

inline
void ConnectOptions::setAllowHalfOpenConnections(bool value)
{
    d_allowHalfOpenConnections = value;
}

inline
void ConnectOptions::setResolutionMode(btlmt::ResolutionMode::Enum value)
{
    d_resolutionMode = value;
}

inline
void ConnectOptions::setSocketOptions(const btlso::SocketOptions& value)
{
    d_socketOptions.makeValue(value);
}

inline
void ConnectOptions::setLocalAddress(const btlso::IPv4Address& value)
{
    d_localAddress.makeValue(value);
}

inline
void ConnectOptions::setSocketPtr(
            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > *value)
{
    d_socketPtr = value;
}

                                  // Aspects

inline
void ConnectOptions::swap(ConnectOptions& other)
{
    BSLS_ASSERT_SAFE(d_serverEndpoint.getAllocator() ==
                                        other.d_serverEndpoint.getAllocator());

    bslalg::SwapUtil::swap(&d_serverEndpoint,     &other.d_serverEndpoint);
    bslalg::SwapUtil::swap(&d_numAttempts,        &other.d_numAttempts);
    bslalg::SwapUtil::swap(&d_timeout,            &other.d_timeout);
    bslalg::SwapUtil::swap(&d_enableRead,         &other.d_enableRead);
    bslalg::SwapUtil::swap(&d_allowHalfOpenConnections,
                           &other.d_allowHalfOpenConnections);
    bslalg::SwapUtil::swap(&d_resolutionMode,     &other.d_resolutionMode);
    bslalg::SwapUtil::swap(&d_socketOptions,      &other.d_socketOptions);
    bslalg::SwapUtil::swap(&d_localAddress,       &other.d_localAddress);
    bslalg::SwapUtil::swap(&d_socketPtr,          &other.d_socketPtr);
}

// ACCESSORS
inline
const btlmt::ConnectOptions::ServerEndpoint&
ConnectOptions::serverEndpoint() const
{
    return d_serverEndpoint;
}

inline
int ConnectOptions::numAttempts() const
{
    return d_numAttempts;
}

inline
const bsls::TimeInterval& ConnectOptions::timeout() const
{
    return d_timeout;
}

inline
bool ConnectOptions::enableRead() const
{
    return d_enableRead;
}

inline
bool ConnectOptions::allowHalfOpenConnections() const
{
    return d_allowHalfOpenConnections;
}

inline
btlmt::ResolutionMode::Enum ConnectOptions::resolutionMode() const
{
    return d_resolutionMode;
}

inline
const bdlb::NullableValue<btlso::SocketOptions>&
ConnectOptions::socketOptions() const
{
    return d_socketOptions;
}

inline
const bdlb::NullableValue<btlso::IPv4Address>&
ConnectOptions::localAddress() const
{
    return d_localAddress;
}

inline
const bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > *
ConnectOptions::socketPtr() const
{
    return d_socketPtr;
}

}  // close package namespace

// FREE FUNCTIONS
inline
bool btlmt::operator==(const btlmt::ConnectOptions& lhs,
                       const btlmt::ConnectOptions& rhs)
{
    return  lhs.serverEndpoint() == rhs.serverEndpoint()
         && lhs.numAttempts() == rhs.numAttempts()
         && lhs.timeout() == rhs.timeout()
         && lhs.enableRead() == rhs.enableRead()
         && lhs.allowHalfOpenConnections() == rhs.allowHalfOpenConnections()
         && lhs.resolutionMode() == rhs.resolutionMode()
         && lhs.socketOptions() == rhs.socketOptions()
         && lhs.localAddress() == rhs.localAddress()
         && lhs.socketPtr() == rhs.socketPtr();
}

inline
bool btlmt::operator!=(const btlmt::ConnectOptions& lhs,
                       const btlmt::ConnectOptions& rhs)
{
    return !(lhs == rhs);
}

inline
void btlmt::swap(ConnectOptions& a, ConnectOptions& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
