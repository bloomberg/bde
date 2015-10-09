// btlso_socketoptions.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_SOCKETOPTIONS
#define INCLUDED_BTLSO_SOCKETOPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic class to store socket options.
//
//@CLASSES:
//  btlso::SocketOptions: container for storing socket options
//
//@SEE_ALSO: btlso_socketimputil, btlso_lingeroptions
//
//@DESCRIPTION: This component provides a class, 'btlso::SocketOptions', that
// contains various options that can be set on a socket.
//
// The options that can be set on a socket are specified below:
//..
//  Option                     btlso::SocketOptUtil   Description
//  ------                     -------------------   -----------
//
//  debugFlag                  BTESO_DEBUGINFO       This option indicates
//                                                   whether debug information
//                                                   should be recorded.
//
//  allowBroadcasting          BTESO_BROADCAST       This option indicates
//                                                   whether broadcast messages
//                                                   can be sent.
//
//  reuseAddress               BTESO_REUSEADDRESS    This option indicates
//                                                   whether local addresses
//                                                   can be reused.
//
//  keepAlive                  BTESO_KEEPALIVE       This option indicates
//                                                   whether connections should
//                                                   be kept alive by enabling
//                                                   periodic transmission of
//                                                   messages.  Not support for
//                                                   stream sockets on windows.
//
//  bypassNormalRouting        BTESO_DONTROUTE       This option indicates
//                                                   whether out-going messages
//                                                   should bypass normal
//                                                   message routing
//                                                   facilities.
//
//  linger                     BTESO_LINGER          See 'btlso_lingeroptions'
//                                                   for details on the linger
//                                                   options.
//
//  leaveOutOfBandDataInline   BTESO_OOBINLINE       This option indicates
//                                                   whether out-of-band data
//                                                   (i.e., data marked urgent)
//                                                   should be left in the
//                                                   normal input queue.
//
//  sendBufferSize             BTESO_SENDBUFFER      This option specifies the
//                                                   size of the output buffer
//                                                   (in bytes).
//
//  receiveBufferSize          BTESO_RECEIVEBUFFER   This option specifies the
//                                                   size of the input buffer
//                                                   (in bytes).
//
//  minimumSendBufferSize      BTESO_SENDLOWATER     This option specifies the
//                                                   minimum number of bytes
//                                                   that must be cached before
//                                                   data is passed to lower
//                                                   layers.
//
//  minimumReceiveBufferSize   BTESO_RECEIVELOWATER  This option specifies the
//                                                   minimum number of bytes
//                                                   that must be cached before
//                                                   data is passed to the
//                                                   application layer.
//
//  sendTimeout                BTESO_SENDTIMEOUT     This option specifies the
//                                                   timeout (in seconds) for
//                                                   blocking send calls.
//
//  receiveTimeout             BTESO_RECEIVETIMEOUT  This option specifies the
//                                                   timeout (in seconds) for
//                                                   blocking receive calls.
//
//  tcpNoDelay                 BTESO_TCPNODELAY      This option indicates
//                                                   whether the Nagle
//                                                   algorithm for packet
//                                                   coalescing should be
//                                                   disabled.  A value of
//                                                   'true' indicates packet
//                                                   coalescing should be
//                                                   disabled , and packets
//                                                   should be sent as soon as
//                                                   possible even if there is
//                                                   only a small amount of
//                                                   data.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting the Socket Option on a Socket
/// - - - - - - - - - - - - - - - - - - - - - - - -
// 'btlso::SocketOptions' can be used to specify whether local addresses should
// be reused.  The following snippets of code illustrate how to set the
// 'BTESO_REUSEADDRESS' flag on a socket.  Note that we assume that a socket of
// type 'SOCK_STREAM' has already been created and that it can be accessed
// using 'socketHandle':
//..
//  bool                 reuseLocalAddr = true;
//  btlso::SocketOptions options;
//  options.setReuseAddress(reuseLocalAddr);
//  assert(reuseLocalAddr == options.reuseAddress().value());
//
//  // Set 'options' on 'socketHandle'
//
//  // . . .
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_LINGEROPTIONS
#include <btlso_lingeroptions.h>
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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace btlso {

                       // ===================
                       // class SocketOptions
                       // ===================

class SocketOptions {
    // This class provides an value-semantic object that contains the various
    // options that can be set on a socket.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing.  (A precise operational definition of
    // when two objects have the same value can be found in the description of
    // the homogeneous (free) 'operator==' for this class.)  This class is
    // *exception* *safe*, but provides no general guarantee of rollback: If an
    // exception is thrown during the invocation of a method on a pre-existing
    // object, the object will be left in a coherent state, but (unless
    // otherwise stated) its value is not defined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) for the same operation is supported in all
    // cases.

    // DATA
    bdlb::NullableValue<LingerOptions> d_linger;  // should lingering be used

    bdlb::NullableValue<int>           d_sendBufferSize;
                                                  // send buffer size

    bdlb::NullableValue<int>           d_receiveBufferSize;
                                                  // receive buffer size

    bdlb::NullableValue<int>           d_minimumSendBufferSize;
                                                  // minimum number of output
                                                  // bytes

    bdlb::NullableValue<int>           d_minimumReceiveBufferSize;
                                                  // minimum number of input
                                                  // bytes

    bdlb::NullableValue<int>           d_sendTimeout;
                                                  // timeout (in seconds) for
                                                  // sending messages

    bdlb::NullableValue<int>           d_receiveTimeout;
                                                  // timeout (in seconds) for
                                                  // receiving messages

    bdlb::NullableValue<bool>          d_debugFlag;
                                                  // should debug information
                                                  // be recorded

    bdlb::NullableValue<bool>          d_allowBroadcasting;
                                                  // can broadcast messages be
                                                  // sent

    bdlb::NullableValue<bool>          d_reuseAddress;
                                                  // can local addresses be
                                                  // reused

    bdlb::NullableValue<bool>          d_keepAlive;
                                                  // should connections be
                                                  // kept alive by enabling
                                                  // periodic transmission of
                                                  // messages

    bdlb::NullableValue<bool>          d_bypassNormalRouting;
                                                  // should out-going messages
                                                  // bypass normal message
                                                  // routing facilities

    bdlb::NullableValue<bool>          d_leaveOutOfBandDataInline;
                                                  // should out-of-band data
                                                  // (data marked urgent) be
                                                  // left inline

    bdlb::NullableValue<bool>          d_tcpNoDelay;
                                                  // disable the Nagle
                                                  // algorithm for send
                                                  // coalescing

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SocketOptions, bslmf::IsBitwiseMoveable)

    // CREATORS
    SocketOptions();
        // Create a socket options object having none of the option values
        // non-null.

    SocketOptions(const SocketOptions& original);
        // Create a socket options object having the value of the specified
        // 'original' object.

    ~SocketOptions();
        // Destroy this object.

    // MANIPULATORS
    SocketOptions& operator=(const SocketOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this socket options object to the default value (i.e., its
        // value upon default construction).

    void setDebugFlag(bool value);
        // Set the 'debugFlag' attribute of this object to the specified
        // 'value'.

    void setAllowBroadcasting(bool value);
        // Set the 'allowBroadcasting' attribute of this object to the
        // specified 'value'.

    void setReuseAddress(bool value);
        // Set the 'reuseAddress' attribute of this object to the specified
        // 'value'.

    void setKeepAlive(bool value);
        // Set the 'keepAlive' attribute of this object to the specified
        // 'value'.

    void setBypassNormalRouting(bool value);
        // Set the 'bypassNormalRouting' attribute of this object to the
        // specified 'value'.

    void setLinger(const LingerOptions& value);
        // Set the 'linger' attribute of this object to the specified 'value'.

    void setLeaveOutOfBandDataInline(bool value);
        // Set the 'leaveOutOfBandDataInline' attribute of this object to the
        // specified 'value'.

    void setSendBufferSize(int value);
        // Set the 'sendBufferSize' attribute of this object to the specified
        // 'value'.

    void setReceiveBufferSize(int value);
        // Set the 'receiveBufferSize' attribute of this object to the
        // specified 'value'.

    void setMinimumSendBufferSize(int value);
        // Set the 'minimumSendBufferSize' attribute of this object to the
        // specified 'value'.

    void setMinimumReceiveBufferSize(int value);
        // Set the 'minimumReceiveBufferSize' attribute of this object to the
        // specified 'value'.

    void setSendTimeout(int value);
        // Set the 'sendTimeout' attribute of this object to the specified
        // 'value'.

    void setReceiveTimeout(int value);
        // Set the 'receiveTimeout' attribute of this object to the specified
        // 'value'.

    void setTcpNoDelay(bool value);
        // Set the 'tcpNoDelay' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
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

    const bdlb::NullableValue<bool>& debugFlag() const;
        // Return a reference to the non-modifiable 'debugFlag' attribute.

    const bdlb::NullableValue<bool>& allowBroadcasting() const;
        // Return a reference to the non-modifiable 'allowBroadcasting
        // attribute.

    const bdlb::NullableValue<bool>& reuseAddress() const;
        // Return a reference to the non-modifiable 'reuseAddress' attribute.

    const bdlb::NullableValue<bool>& keepAlive() const;
        // Return a reference to the non-modifiable 'keepAlive' attribute.

    const bdlb::NullableValue<bool>& bypassNormalRouting() const;
        // Return a reference to the non-modifiable 'bypassNormalRouting'
        // attribute.

    const bdlb::NullableValue<LingerOptions>& linger() const;
        // Return a reference to the non-modifiable 'linger' attribute.

    const bdlb::NullableValue<bool>& leaveOutOfBandDataInline() const;
        // Return a reference to the non-modifiable 'leaveOutOfBandDataInline'
        // attribute.

    const bdlb::NullableValue<int>& sendBufferSize() const;
        // Return a reference to the non-modifiable 'sendBufferSize' attribute.

    const bdlb::NullableValue<int>& receiveBufferSize() const;
        // Return a reference to the non-modifiable 'receiveBufferSize'
        // attribute.

    const bdlb::NullableValue<int>& minimumSendBufferSize() const;
        // Return a reference to the non-modifiable 'minimumSendBufferSize'
        // attribute.

    const bdlb::NullableValue<int>& minimumReceiveBufferSize() const;
        // Return a reference to the non-modifiable 'minimumReceiveBufferSize'
        // attribute.

    const bdlb::NullableValue<int>& sendTimeout() const;
        // Return a reference to the non-modifiable 'sendTimeout' attribute.

    const bdlb::NullableValue<int>& receiveTimeout() const;
        // Return a reference to the non-modifiable 'receiveTimeout' attribute.

    const bdlb::NullableValue<bool>& tcpNoDelay() const;
        // Return a reference to the non-modifiable 'tcpNoDelay' attribute.
};

// FREE OPERATORS
inline
bool operator==(const SocketOptions& lhs, const SocketOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SocketOptions& lhs, const SocketOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SocketOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -------------------
                       // class SocketOptions
                       // -------------------

// MANIPULATORS
inline
void SocketOptions::setDebugFlag(bool value)
{
    d_debugFlag.makeValue(value);
}

inline
void SocketOptions::setAllowBroadcasting(bool value)
{
    d_allowBroadcasting.makeValue(value);
}

inline
void SocketOptions::setReuseAddress(bool value)
{
    d_reuseAddress.makeValue(value);
}

inline
void SocketOptions::setKeepAlive(bool value)
{
    d_keepAlive.makeValue(value);
}

inline
void SocketOptions::setBypassNormalRouting(bool value)
{
    d_bypassNormalRouting.makeValue(value);
}

inline
void SocketOptions::setLinger(const LingerOptions& value)
{
    d_linger.makeValue(value);
}

inline
void SocketOptions::setLeaveOutOfBandDataInline(bool value)
{
    d_leaveOutOfBandDataInline.makeValue(value);
}

inline
void SocketOptions::setSendBufferSize(int value)
{
    d_sendBufferSize.makeValue(value);
}

inline
void SocketOptions::setReceiveBufferSize(int value)
{
    d_receiveBufferSize.makeValue(value);
}

inline
void SocketOptions::setMinimumSendBufferSize(int value)
{
    d_minimumSendBufferSize.makeValue(value);
}

inline
void SocketOptions::setMinimumReceiveBufferSize(int value)
{
    d_minimumReceiveBufferSize.makeValue(value);
}

inline
void SocketOptions::setSendTimeout(int value)
{
    d_sendTimeout.makeValue(value);
}

inline
void SocketOptions::setReceiveTimeout(int value)
{
    d_receiveTimeout.makeValue(value);
}

inline
void SocketOptions::setTcpNoDelay(bool value)
{
    d_tcpNoDelay.makeValue(value);
}

// ACCESSORS
inline
const bdlb::NullableValue<bool>& SocketOptions::debugFlag() const
{
    return d_debugFlag;
}

inline
const bdlb::NullableValue<bool>& SocketOptions::allowBroadcasting() const
{
    return d_allowBroadcasting;
}

inline
const bdlb::NullableValue<bool>& SocketOptions::reuseAddress() const
{
    return d_reuseAddress;
}

inline
const bdlb::NullableValue<bool>& SocketOptions::keepAlive() const
{
    return d_keepAlive;
}

inline
const bdlb::NullableValue<bool>&
SocketOptions::bypassNormalRouting() const
{
    return d_bypassNormalRouting;
}

inline
const bdlb::NullableValue<LingerOptions>&
SocketOptions::linger() const
{
    return d_linger;
}

inline
const bdlb::NullableValue<bool>&
SocketOptions::leaveOutOfBandDataInline() const
{
    return d_leaveOutOfBandDataInline;
}

inline
const bdlb::NullableValue<int>& SocketOptions::sendBufferSize() const
{
    return d_sendBufferSize;
}

inline
const bdlb::NullableValue<int>& SocketOptions::receiveBufferSize() const
{
    return d_receiveBufferSize;
}

inline
const bdlb::NullableValue<int>&
SocketOptions::minimumSendBufferSize() const
{
    return d_minimumSendBufferSize;
}

inline
const bdlb::NullableValue<int>&
SocketOptions::minimumReceiveBufferSize() const
{
    return d_minimumReceiveBufferSize;
}

inline
const bdlb::NullableValue<int>& SocketOptions::sendTimeout() const
{
    return d_sendTimeout;
}

inline
const bdlb::NullableValue<int>& SocketOptions::receiveTimeout() const
{
    return d_receiveTimeout;
}

inline
const bdlb::NullableValue<bool>& SocketOptions::tcpNoDelay() const
{
    return d_tcpNoDelay;
}

}  // close package namespace

// FREE FUNCTIONS
inline
bool btlso::operator==(const SocketOptions& lhs, const SocketOptions& rhs)
{
    return  lhs.debugFlag()                == rhs.debugFlag()
         && lhs.allowBroadcasting()        == rhs.allowBroadcasting()
         && lhs.reuseAddress()             == rhs.reuseAddress()
         && lhs.keepAlive()                == rhs.keepAlive()
         && lhs.bypassNormalRouting()      == rhs.bypassNormalRouting()
         && lhs.linger()                   == rhs.linger()
         && lhs.leaveOutOfBandDataInline() == rhs.leaveOutOfBandDataInline()
         && lhs.sendBufferSize()           == rhs.sendBufferSize()
         && lhs.receiveBufferSize()        == rhs.receiveBufferSize()
         && lhs.minimumSendBufferSize()    == rhs.minimumSendBufferSize()
         && lhs.minimumReceiveBufferSize() == rhs.minimumReceiveBufferSize()
         && lhs.sendTimeout()              == rhs.sendTimeout()
         && lhs.receiveTimeout()           == rhs.receiveTimeout()
         && lhs.tcpNoDelay()               == rhs.tcpNoDelay();
}

inline
bool btlso::operator!=(const SocketOptions& lhs, const SocketOptions& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& btlso::operator<<(bsl::ostream& stream, const SocketOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

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
