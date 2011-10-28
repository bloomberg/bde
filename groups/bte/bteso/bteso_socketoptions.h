// bteso_socketoptions.h                                              -*-C++-*-
#ifndef INCLUDED_BTESO_SOCKETOPTIONS
#define INCLUDED_BTESO_SOCKETOPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic class to store socket options.
//
//@CLASSES:
//  bteso_SocketOptions: container for storing socket options
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bteso_socketimputil, bteso_lingeroptions
//
//@DESCRIPTION: This component provides a class, 'bteso_SocketOptions', that
// contains various options that can be set on a socket.
//
// The options that can be set on a socket are specified below:
//..
//  Option                     bteso_SocketOptUtil   Description
//  ------                     -------------------   -----------
//
//  debugFlag                  BTESO_DEBUGINFO       This option indicates
//                                                   whether debug information
//                                                   should be recorded.
//
//  allowBroadcasting          BTESO_BROADCAST       This option indicates
//                                                   whether broadcast messages
//                                                   can be sent.  Not supported
//                                                   for datagram sockets on
//                                                   windows.
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
//  linger                     BTESO_LINGER          See 'bteso_lingeroptions'
//                                                   for details on the linger
//                                                   options.
//
//  leaveOutOfBandDataInline   BTESO_OOBINLINE       This option indicates
//                                                   whether out-of-band data
//                                                   (i.e., data marked urgent)
//                                                   should be left in the
//                                                   normal input queue.  Not
//                                                   supported for stream
//                                                   sockets on Linux or
//                                                   Windows.
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
//..
//
///Usage
///-----
// 'bteso_SocketOptions' can be used to specify whether local addresses should
// be reused.  The following snippets of code illustrate how to set the
// 'BTESO_REUSEADDRESS' flag on a socket.  Note that we assume that a socket
// of type 'SOCK_STREAM' has already been created and that it can be accessed
// using 'socketHandle':
//..
//  bool                reuseLocalAddr = true;
//  bteso_SocketOptions options;
//  options.reuseAddress().makeValue(reuseLocalAddr);
//
//  int rc = bteso_SocketOptUtil::setSocketOptions(socketHandle, options);
//  assert(0 == rc);
//..
// Now we will verify that the address option was set correctly:
//..
//  int addropt = 0;
//  rc = bteso_SocketOptUtil::getOption(
//                                    &addropt,
//                                    socketHandle,
//                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                                    bteso_SocketOptUtil::BTESO_REUSEADDRESS);
//
//  assert(0 == rc);
//  assert(0 != addropt);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_LINGEROPTIONS
#include <bteso_lingeroptions.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                       // =========================
                       // class bteso_SocketOptions
                       // =========================

class bteso_SocketOptions {
    // This class provides an value-semantic object that contains the various
    // options that can be set on a socket.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two objects have the same value can be
    // found in the description of the homogeneous (free) 'operator==' for this
    // class.)  This class is *exception* *safe*, but provides no general
    // guarantee of rollback: If an exception is thrown during the invocation
    // of a method on a pre-existing object, the object will be left in a
    // coherent state, but (unless otherwise stated) its value is not defined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) for the same operation
    // is supported in all cases.

    // DATA
    bdeut_NullableValue<bteso_LingerOptions>
                               d_linger; // should lingering be used

    bdeut_NullableValue<int>   d_sendBufferSize;
                                         // send buffer size

    bdeut_NullableValue<int>   d_receiveBufferSize;
                                         // receive buffer size

    bdeut_NullableValue<int>   d_minimumSendBufferSize;
                                         // minimum number of output bytes

    bdeut_NullableValue<int>   d_minimumReceiveBufferSize;
                                         // minimum number of input bytes

    bdeut_NullableValue<int>   d_sendTimeout;
                                         // timeout (in seconds) for sending
                                         // messages

    bdeut_NullableValue<int>   d_receiveTimeout;
                                         // timeout (in seconds) for receiving
                                         // messages

    bdeut_NullableValue<bool>  d_debugFlag;
                                         // should debug information be
                                         // recorded

    bdeut_NullableValue<bool>  d_allowBroadcasting;
                                         // can broadcast messages be sent

    bdeut_NullableValue<bool>  d_reuseAddress;
                                         // can local addresses be reused

    bdeut_NullableValue<bool>  d_keepAlive;
                                         // should connections be kept alive
                                         // by enabling periodic transmission
                                         // of messages.

    bdeut_NullableValue<bool>  d_bypassNormalRouting;
                                         // should out-going messages bypass
                                         // normal message routing facilities

    bdeut_NullableValue<bool>  d_leaveOutOfBandDataInline;
                                         // should out-of-band data (data
                                         // marked urgent) be left inline

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_SocketOptions,
                                 bslalg_TypeTraitBitwiseMoveable);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    bteso_SocketOptions();
        // Create a socket options object having none of the option values
        // non-null.

    bteso_SocketOptions(const bteso_SocketOptions& original);
        // Create a socket options object having the value of the specified
        // 'original' object.

    ~bteso_SocketOptions();
        // Destroy this object.

    // MANIPULATORS
    bteso_SocketOptions& operator=(const bteso_SocketOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

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

    void setLinger(const bteso_LingerOptions& value);
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    const bdeut_NullableValue<bool>& debugFlag() const;
        // Return a reference to the non-modifiable 'debugFlag' attribute.

    const bdeut_NullableValue<bool>& allowBroadcasting() const;
        // Return a reference to the non-modifiable 'allowBroadcasting
        // attribute.

    const bdeut_NullableValue<bool>& reuseAddress() const;
        // Return a reference to the non-modifiable 'reuseAddress' attribute.

    const bdeut_NullableValue<bool>& keepAlive() const;
        // Return a reference to the non-modifiable 'keepAlive' attribute.

    const bdeut_NullableValue<bool>& bypassNormalRouting() const;
        // Return a reference to the non-modifiable 'bypassNormalRouting'
        // attribute.

    const bdeut_NullableValue<bteso_LingerOptions>& linger() const;
        // Return a reference to the non-modifiable 'linger' attribute.

    const bdeut_NullableValue<bool>& leaveOutOfBandDataInline() const;
        // Return a reference to the non-modifiable 'leaveOutOfBandDataInline'
        // attribute.

    const bdeut_NullableValue<int>& sendBufferSize() const;
        // Return a reference to the non-modifiable 'sendBufferSize' attribute.

    const bdeut_NullableValue<int>& receiveBufferSize() const;
        // Return a reference to the non-modifiable 'receiveBufferSize'
        // attribute.

    const bdeut_NullableValue<int>& minimumSendBufferSize() const;
        // Return a reference to the non-modifiable 'minimumSendBufferSize'
        // attribute.

    const bdeut_NullableValue<int>& minimumReceiveBufferSize() const;
        // Return a reference to the non-modifiable 'minimumReceiveBufferSize'
        // attribute.

    const bdeut_NullableValue<int>& sendTimeout() const;
        // Return a reference to the non-modifiable 'sendTimeout' attribute.

    const bdeut_NullableValue<int>& receiveTimeout() const;
        // Return a reference to the non-modifiable 'receiveTimeout' attribute.
};

// FREE OPERATORS
inline
bool operator==(const bteso_SocketOptions& lhs,
                const bteso_SocketOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const bteso_SocketOptions& lhs,
                const bteso_SocketOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bteso_SocketOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -------------------------
                       // class bteso_SocketOptions
                       // -------------------------

// CLASS METHODS
inline
int bteso_SocketOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& bteso_SocketOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_debugFlag, 1);
            bdex_InStreamFunctions::streamIn(stream, d_allowBroadcasting, 1);
            bdex_InStreamFunctions::streamIn(stream, d_reuseAddress, 1);
            bdex_InStreamFunctions::streamIn(stream, d_keepAlive, 1);
            bdex_InStreamFunctions::streamIn(stream, d_bypassNormalRouting, 1);
            bdex_InStreamFunctions::streamIn(stream, d_linger, 1);
            bdex_InStreamFunctions::streamIn(stream,
                                             d_leaveOutOfBandDataInline,
                                             1);
            bdex_InStreamFunctions::streamIn(stream, d_sendBufferSize, 1);
            bdex_InStreamFunctions::streamIn(stream, d_receiveBufferSize, 1);
            bdex_InStreamFunctions::streamIn(stream,
                                             d_minimumSendBufferSize,
                                             1);
            bdex_InStreamFunctions::streamIn(stream,
                                             d_minimumReceiveBufferSize,
                                             1);
            bdex_InStreamFunctions::streamIn(stream, d_sendTimeout, 1);
            bdex_InStreamFunctions::streamIn(stream, d_receiveTimeout, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bteso_SocketOptions::setDebugFlag(bool value)
{
    d_debugFlag.makeValue(value);
}

inline
void bteso_SocketOptions::setAllowBroadcasting(bool value)
{
    d_allowBroadcasting.makeValue(value);
}

inline
void bteso_SocketOptions::setReuseAddress(bool value)
{
    d_reuseAddress.makeValue(value);
}

inline
void bteso_SocketOptions::setKeepAlive(bool value)
{
    d_keepAlive.makeValue(value);
}

inline
void bteso_SocketOptions::setBypassNormalRouting(bool value)
{
    d_bypassNormalRouting.makeValue(value);
}

inline
void bteso_SocketOptions::setLinger(const bteso_LingerOptions& value)
{
    d_linger.makeValue(value);
}

inline
void bteso_SocketOptions::setLeaveOutOfBandDataInline(bool value)
{
    d_leaveOutOfBandDataInline.makeValue(value);
}

inline
void bteso_SocketOptions::setSendBufferSize(int value)
{
    d_sendBufferSize.makeValue(value);
}

inline
void bteso_SocketOptions::setReceiveBufferSize(int value)
{
    d_receiveBufferSize.makeValue(value);
}

inline
void bteso_SocketOptions::setMinimumSendBufferSize(int value)
{
    d_minimumSendBufferSize.makeValue(value);
}

inline
void bteso_SocketOptions::setMinimumReceiveBufferSize(int value)
{
    d_minimumReceiveBufferSize.makeValue(value);
}

inline
void bteso_SocketOptions::setSendTimeout(int value)
{
    d_sendTimeout.makeValue(value);
}

inline
void bteso_SocketOptions::setReceiveTimeout(int value)
{
    d_receiveTimeout.makeValue(value);
}

// ACCESSORS
template <class STREAM>
STREAM& bteso_SocketOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_debugFlag, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_allowBroadcasting, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_reuseAddress, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_keepAlive, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_bypassNormalRouting, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_linger, 1);
        bdex_OutStreamFunctions::streamOut(stream,
                                           d_leaveOutOfBandDataInline,
                                           1);
        bdex_OutStreamFunctions::streamOut(stream, d_sendBufferSize, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_receiveBufferSize, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_minimumSendBufferSize, 1);
        bdex_OutStreamFunctions::streamOut(stream,
                                           d_minimumReceiveBufferSize,
                                           1);
        bdex_OutStreamFunctions::streamOut(stream, d_sendTimeout, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_receiveTimeout, 1);
      } break;
    }
    return stream;
}

inline
const bdeut_NullableValue<bool>& bteso_SocketOptions::debugFlag() const
{
    return d_debugFlag;
}

inline
const bdeut_NullableValue<bool>& bteso_SocketOptions::allowBroadcasting() const
{
    return d_allowBroadcasting;
}

inline
const bdeut_NullableValue<bool>& bteso_SocketOptions::reuseAddress() const
{
    return d_reuseAddress;
}

inline
const bdeut_NullableValue<bool>& bteso_SocketOptions::keepAlive() const
{
    return d_keepAlive;
}

inline
const bdeut_NullableValue<bool>&
bteso_SocketOptions::bypassNormalRouting() const
{
    return d_bypassNormalRouting;
}

inline
const bdeut_NullableValue<bteso_LingerOptions>&
bteso_SocketOptions::linger() const
{
    return d_linger;
}

inline
const bdeut_NullableValue<bool>&
bteso_SocketOptions::leaveOutOfBandDataInline() const
{
    return d_leaveOutOfBandDataInline;
}

inline
const bdeut_NullableValue<int>& bteso_SocketOptions::sendBufferSize() const
{
    return d_sendBufferSize;
}

inline
const bdeut_NullableValue<int>& bteso_SocketOptions::receiveBufferSize() const
{
    return d_receiveBufferSize;
}

inline
const bdeut_NullableValue<int>&
bteso_SocketOptions::minimumSendBufferSize() const
{
    return d_minimumSendBufferSize;
}

inline
const bdeut_NullableValue<int>&
bteso_SocketOptions::minimumReceiveBufferSize() const
{
    return d_minimumReceiveBufferSize;
}

inline
const bdeut_NullableValue<int>& bteso_SocketOptions::sendTimeout() const
{
    return d_sendTimeout;
}

inline
const bdeut_NullableValue<int>& bteso_SocketOptions::receiveTimeout() const
{
    return d_receiveTimeout;
}

// FREE FUNCTIONS
inline
bool operator==(const bteso_SocketOptions& lhs,
                const bteso_SocketOptions& rhs)
{
    return  lhs.debugFlag() == rhs.debugFlag()
         && lhs.allowBroadcasting() == rhs.allowBroadcasting()
         && lhs.reuseAddress() == rhs.reuseAddress()
         && lhs.keepAlive() == rhs.keepAlive()
         && lhs.bypassNormalRouting() == rhs.bypassNormalRouting()
         && lhs.linger() == rhs.linger()
         && lhs.leaveOutOfBandDataInline() == rhs.leaveOutOfBandDataInline()
         && lhs.sendBufferSize() == rhs.sendBufferSize()
         && lhs.receiveBufferSize() == rhs.receiveBufferSize()
         && lhs.minimumSendBufferSize() == rhs.minimumSendBufferSize()
         && lhs.minimumReceiveBufferSize() == rhs.minimumReceiveBufferSize()
         && lhs.sendTimeout() == rhs.sendTimeout()
         && lhs.receiveTimeout() == rhs.receiveTimeout();
}

inline
bool operator!=(const bteso_SocketOptions& lhs,
                const bteso_SocketOptions& rhs)
{
    return  lhs.debugFlag() != rhs.debugFlag()
         || lhs.allowBroadcasting() != rhs.allowBroadcasting()
         || lhs.reuseAddress() != rhs.reuseAddress()
         || lhs.keepAlive() != rhs.keepAlive()
         || lhs.bypassNormalRouting() != rhs.bypassNormalRouting()
         || lhs.linger() != rhs.linger()
         || lhs.leaveOutOfBandDataInline() != rhs.leaveOutOfBandDataInline()
         || lhs.sendBufferSize() != rhs.sendBufferSize()
         || lhs.receiveBufferSize() != rhs.receiveBufferSize()
         || lhs.minimumSendBufferSize() != rhs.minimumSendBufferSize()
         || lhs.minimumReceiveBufferSize() != rhs.minimumReceiveBufferSize()
         || lhs.sendTimeout() != rhs.sendTimeout()
         || lhs.receiveTimeout() != rhs.receiveTimeout();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const bteso_SocketOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
