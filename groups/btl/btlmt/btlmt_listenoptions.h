// btlmt_listenoptions.h                                              -*-C++-*-
#ifndef INCLUDED_BTLMT_LISTENOPTIONS
#define INCLUDED_BTLMT_LISTENOPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide attribute class for characterizing server 'listen' options.
//
//@CLASSES:
//  btlmt::ListenOptions: attributes characterizing 'listen' options
//
//@SEE_ALSO: btlmt_channelpool
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'btlmt::ListenOptions', that is used to
// characterize options for listening servers.
//
///Attributes
///----------
//..
//  Name                     Type                                   Constraints
//  ------                   ----                                   -----------
//  serverAddress            btlso::IPv4Address                       none
//  backlog                  int                                      > 0
//  timeout                  bdlb::NullableValue<bsls::TimeInterval>  none
//  enableRead               bool                                     none
//  allowHalfOpenConnections bool                                     none
//  socketOptions            btlso::SocketOptions                     none
//..
//: o 'serverAddress':
//:   o Type: btlso::IPv4Address
//:   o Default: btlso::IPv4Address()
//:   o Constraints: none
//:   o Description: IP Address that the server should listen on.
//:
//: o 'backlog':
//:   o Type: int
//:   o Default: 1
//:   o Constraints: > 0
//:   o Description: maximum number of pending connections on the listening
//:     socket.
//:
//: o 'timeout':
//:   o Type: bdlb::NullableValue<bsls::TimeInterval>
//:   o Default: null
//:   o Constraints: none
//:   o Description: the time duration to wait to accept a connection.
//:
//: o 'enableRead':
//:   o Type: bool
//:   o Default: true
//:   o Constraints: none
//:   o Description: flag specifying if read operations should be enabled for
//:     accepted connections.  A 'true' value for this flag implies that
//:     incoming data on new connections will be automatically monitored and a
//:     'false' value implies that incoming data will be ignored.
//:
//: o 'allowHalfOpenConnections':
//:   o Type: bool
//:   o Default: false
//:   o Constraints: none
//:   o Description: flag specifying if half-open connections are
//:     allowed.  A connection is half-open if only one of its receive or send
//:     ends (but not both) is active at a time.  If this flag is 'true' and a
//:     connection closes its send part but keeps its receive part open, then
//:     outgoing data will no longer be sent but incoming data will continue to
//:     read.  Similarly, if this flag is 'true' and a connection closes its
//:     receive part but keeps its send part open, then any queued data will
//:     continue to be sent but no new data is read.  If this flag is 'false'
//:     then closure of any end of the connection will result in both ends of
//:     the connection being closed.
//:
//: o 'socketOptions':
//:   o Type: btlso::SocketOptions
//:   o Default: btlso::SocketOptions()
//:   o Constraints: none
//:   o Description: socket options to use for the listening socket.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a listening server socket
/// - - - - - - - - - - - - - - - - - - - - - -
// This component allows specifying configuration options for creating
// listening sockets.  Consider that we want to set up a server application on
// the local machine that responds to incoming client requests.  The following
// example will show how to provide configuration options for doing so.
//
// We will assume that we have two functions: 1) a 'listen' method that accepts
// a 'btlmt::ListenOptions' object as an argument, creates a listening socket,
// and invokes a specified callback on incoming connections (or timeout), and
// 2) a 'acceptCallback' method that will be supplied to 'listen' to invoke
// when new sockets are accepted.  The signature of both functions is provided
// below but for brevity the implementation is elided (note that in practice
// clients would likely use a higher level component like 'btlmt_sessionpool'
// or 'btlmt_channelpool'):
//..
//  void acceptCallback(int status, int acceptFd)
//      // Process the requests on the connection identified by the specified
//      // 'acceptFd' socket fd if 'status' is 0 and log an error otherwise.
//  {
//      // . . .
//  }
//
//  int listen(const btlmt::ListenOptions&   options,
//             bsl::function<void(int, int)> callback)
//      // Create a listening socket based on the specified listen 'options'
//      // and wait for incoming connections.  Invoke the specified 'callback'
//      // with a callback status and the socket fd of the accepted socket.
//      // The callback status is 0 if a new socket was accepted and non-zero
//      // otherwise.  If status is non-zero the socket fd passed to 'callback'
//      // is -1.  Return 0 on success and a non-zero value otherewise.  The
//      // 'callback' parameter is invoked as follows:
//      //..
//      //  void callback(int status, int acceptFd);
//      //..
//  {
//      // . . .
//
//      return 0;
//  }
//..
// First, we will construct the connect options object, and verify its default
// constructed state:
//..
//  btlmt::ListenOptions options;
//  assert(btlso::IPv4Address()   == options.serverAddress());
//  assert(1                      == options.backlog());
//  assert(true                   == options.enableRead());
//  assert(false                  == options.allowHalfOpenConnections());
//  assert(btlso::SocketOptions() == options.socketOptions());
//  assert(options.timeout().isNull());
//..
// Next, we specify the server address that server will listen on:
//..
//  btlso::IPv4Address address;
//  address.setPortNumber(12345);
//
//  options.setServerAddress(address);
//  assert(address == options.serverAddress());
//..
// Then, we will specify the number of connections that can be waiting to be
// accepted:
//..
//  const int BACKLOG = 10;
//  options.setBacklog(BACKLOG);
//  assert(BACKLOG == options.backlog());
//..
// Next, the time to wait for an incoming connection is specified.  If an
// incoming connection does not arrive within that time frame the 'listen'
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
// Now, we specify certain socket options for the listening socket such as
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
//  const int rc = listen(options, acceptCallback);
//  assert(0 == rc);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETOPTIONS
#include <btlso_socketoptions.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace btlmt {

                       // =========================
                       // class btlmt_ListenOptions
                       // =========================

class ListenOptions {
    // This simply unconstrained (value-semantic) attribute class characterizes
    // the set of options for listening on a socket.  See the Attributes
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

    // DATA
    btlso::IPv4Address         d_serverAddress;  // server address

    int                        d_backlog;        // backlog of pending conns

    bdlb::NullableValue<bsls::TimeInterval>
                               d_timeout;        // time to wait

    bool                       d_enableRead;     // enable read operations

    bool                       d_allowHalfOpenConnections;
                                                 // allow half open connections

    btlso::SocketOptions       d_socketOptions;  // socket options

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ListenOptions, bslmf::IsBitwiseMoveable);

    // CREATORS
    ListenOptions();
        // Create a socket options object having none of the option values
        // non-null.

    ListenOptions(const ListenOptions& original);
        // Create a socket options object having the value of the specified
        // 'original' object.

    ~ListenOptions();
        // Destroy this object.

    // MANIPULATORS
    ListenOptions& operator=(const ListenOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this socket options object to the default value (i.e., its
        // value upon default construction).

    void setServerAddress(const btlso::IPv4Address& value);
        // Set the 'serverAddress' attribute of this object to the specified
        // 'value'.

    void setBacklog(int value);
        // Set the 'backlog' attribute of this object to the specified 'value'.
        // The behavior is undefined unless '0 < backlog'.

    void setTimeout(const bsls::TimeInterval& value);
        // Set the 'timeout' attribute of this object to the specified 'value'.

    void setEnableRead(bool value);
        // Set the 'enableRead' attribute of this object to the specified
        // 'value'.

    void setAllowHalfOpenConnections(bool value);
        // Set the 'allowHalfOpenConnections' attribute of this object to the
        // specified 'value'.

    void setSocketOptions(const btlso::SocketOptions& value);
        // Set the 'socketOptions' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    const btlso::IPv4Address& serverAddress() const;
        // Return a reference to the non-modifiable 'serverAddress' attribute.

    int backlog() const;
        // Return the value of the 'backlog' attribute.

    const bdlb::NullableValue<bsls::TimeInterval>& timeout() const;
        // Return a reference to the non-modifiable 'timeout' attribute.

    bool enableRead() const;
        // Return the value of the 'enableRead' attribute.

    bool allowHalfOpenConnections() const;
        // Return the value of the 'allowHalfOpenConnections' attribute.

    const btlso::SocketOptions& socketOptions() const;
        // Return a reference to the non-modifiable 'socketOptions' attribute.

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
bool operator==(const btlmt::ListenOptions& lhs,
                const btlmt::ListenOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const btlmt::ListenOptions& lhs,
                const btlmt::ListenOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream& stream, const ListenOptions& object);
    // Format the specified 'object' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -------------------------
                       // class btlmt_ListenOptions
                       // -------------------------

// CREATORS
inline
ListenOptions::ListenOptions(const ListenOptions& original)
: d_serverAddress(original.d_serverAddress)
, d_backlog(original.d_backlog)
, d_timeout(original.d_timeout)
, d_enableRead(original.d_enableRead)
, d_allowHalfOpenConnections(original.d_allowHalfOpenConnections)
, d_socketOptions(original.d_socketOptions)
{
}

inline
ListenOptions::~ListenOptions()
{
}

// MANIPULATORS
inline
void ListenOptions::setServerAddress(const btlso::IPv4Address& value)
{
    d_serverAddress = value;
}

inline
void ListenOptions::setBacklog(int value)
{
    BSLS_ASSERT_SAFE(0 < value);

    d_backlog = value;
}

inline
void ListenOptions::setTimeout(const bsls::TimeInterval& value)
{
    d_timeout = value;
}

inline
void ListenOptions::setEnableRead(bool value)
{
    d_enableRead = value;
}

inline
void ListenOptions::setAllowHalfOpenConnections(bool value)
{
    d_allowHalfOpenConnections = value;
}

inline
void ListenOptions::setSocketOptions(const btlso::SocketOptions& value)
{
    d_socketOptions = value;
}

// ACCESSORS
inline
const btlso::IPv4Address& ListenOptions::serverAddress() const
{
    return d_serverAddress;
}

inline
int ListenOptions::backlog() const
{
    return d_backlog;
}

inline
const bdlb::NullableValue<bsls::TimeInterval>& ListenOptions::timeout() const
{
    return d_timeout;
}

inline
bool ListenOptions::enableRead() const
{
    return d_enableRead;
}

inline
bool ListenOptions::allowHalfOpenConnections() const
{
    return d_allowHalfOpenConnections;
}

inline
const btlso::SocketOptions& ListenOptions::socketOptions() const
{
    return d_socketOptions;
}

}  // close package namespace

// FREE FUNCTIONS
inline
bool btlmt::operator==(const btlmt::ListenOptions& lhs,
                       const btlmt::ListenOptions& rhs)
{
    return  lhs.serverAddress() == rhs.serverAddress()
         && lhs.backlog() == rhs.backlog()
         && lhs.timeout() == rhs.timeout()
         && lhs.enableRead() == rhs.enableRead()
         && lhs.allowHalfOpenConnections() == rhs.allowHalfOpenConnections()
         && lhs.socketOptions() == rhs.socketOptions();
}

inline
bool btlmt::operator!=(const btlmt::ListenOptions& lhs,
                       const btlmt::ListenOptions& rhs)
{
    return !(lhs == rhs);
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
