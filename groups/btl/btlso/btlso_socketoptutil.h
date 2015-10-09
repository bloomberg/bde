// btlso_socketoptutil.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_SOCKETOPTUTIL
#define INCLUDED_BTLSO_SOCKETOPTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide operations to manipulate socket options.
//
//@CLASSES:
//   btlso::SocketOptUtil: namespace for socket options utilities
//
//@SEE_ALSO: btlso_socketimputil btlso_ioutil
//
//@DESCRIPTION: This component provides a namespace for a collection of pure
// procedures to manipulate options on sockets.  These options are enumerated
// for non-platform-specific option classifications.  These options can exist
// at multiple levels such as 'SOL_SOCKET', 'IPPROTO_TCP' and 'IPPROTO_IP'.
// The supported levels include 'SOL_SOCKET' and 'IPPROTO_TCP'.  This component
// acts as a pass-through between the system and the application.
// Particularly, the option name and option level are passed directly (i.e.,
// without any modification) into system calls.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting 'k_REUSEADDRESS' Option on a Socket
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to set 'k_REUSEADDRESS' flag
// on a socket.  Note that we assume that a socket of type 'SOCK_STREAM' is
// already created and socket allocation 'btlso_socketimputil' can be used.
//..
//  int result;
//  int addropt = 1;
//  result = btlso::SocketOptUtil::setOption(
//                                        sockethandle,
//                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
//                                        btlso::SocketOptUtil::k_REUSEADDRESS,
//                                        addropt);
//
//  assert(0 == result);
//..
// Now we will verify that the address option was set correctly:
//..
//  addropt = 0;
//  result = btlso::SocketOptUtil::getOption(
//                                       &addropt,
//                                       sockethandle,
//                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
//                                       btlso::SocketOptUtil::k_REUSEADDRESS);
//
//  assert(0 == result);
//  assert(0 != addropt);
//..
// Next use 'getOption' to verify the type of the socket ('SOCK_STREAM'):
//..
//  int sockettype = 0;
//  result = btlso::SocketOptUtil::getOption(
//                                         &sockettype,
//                                         sockethandle,
//                                         btlso::SocketOptUtil::k_SOCKETLEVEL,
//                                         btlso::SocketOptUtil::k_TYPE);
//
//  assert(0 == result);
//  assert(SOCK_STREAM == sockettype);
//..
//
///Example 2: Setting Linger Options
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to set and get linger data
// option values for a socket.  First set the linger interval to 2 seconds:
//..
//  int                              errorcode = 0;
//  btlso::SocketOptUtil::LingerData ld;
//
//  ld.l_onoff  = 1;   // Enable lingering for
//  ld.l_linger = 2;   // 2 seconds
//
//  result = btlso::SocketOptUtil::setOption(
//                                          sockethandle,
//                                          btlso::SocketOptUtil::k_SOCKETLEVEL
//                                          btlso::SocketOptUtil::k_LINGER,
//                                          ld,
//                                          &errorcode);
//  assert(0 == result);
//  assert(0 == errorcode);
//..
// Now query that the option was actually set:
//..
//  btlso::SocketOptUtil::LingerData ld2
//  result = btlso::SocketOptUtil::getOption(
//                                         &ld2,
//                                         sockethandle,
//                                         btlso::SocketOptUtil::k_SOCKETLEVEL,
//                                         btlso::SocketOptUtil::k_LINGER,
//                                         &errorcode);
//
//  assert(0 == result);
//  assert(0 == errorcode);
//..
//
///Example 3: Illustrating Pass-Through Ability
/// - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the pass-through ability.
//..
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//
//  WSAPROTOCOL_INFO protoInfo;
//  memset(&ProtoInfo, 0, sizeof(ProtoInfo));
//  result = btlso::SocketOptUtil::getOption(
//                                         &protoInfo,
//                                         sockethandle,
//                                         btlso::SocketOptUtil::k_SOCKETLEVEL,
//                                         SO_PROTOCOL_INFO,
//                                         &errorcode);
//
//  assert(0 == result);
//  assert(0 == errorcode);
//  assert(SOCK_STREAM == ProtoInfo.iAddressFamily);
//  assert(ProtocolInformation == ProtoInfo.iProtocol);
//  assert(ProtocolVersion == ProtoInfo.iVersion);
//  assert(SocketType == ProtoInfo.iSocketType);
//
//  #endif
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX

    #ifdef BSLS_PLATFORM_OS_AIX
        #ifndef INCLUDED_SYS_MACHINE
        #include <sys/machine.h>       // Defines "endianness" of the platform.
        #define INCLUDED_SYS_MACHINE
        #endif
    #endif

    #ifndef INCLUDED_SYS_TYPES
    #include <sys/types.h>
    #define INCLUDED_SYS_TYPES
    #endif

    #ifndef INCLUDED_BSL_C_ERRNO
    #include <bsl_c_errno.h>
    #endif

    #ifndef INCLUDED_SYS_SOCKET
    #include <sys/socket.h>
    #define INCLUDED_SYS_SOCKET
    #endif

    #ifndef INCLUDED_NETINET_TCP
    #include <netinet/tcp.h>
    #define INCLUDED_NETINET_TCP
    #endif

    #ifndef INCLUDED_NETINET_IN
    #include <netinet/in.h>
    #define INCLUDED_NETINET_IN
    #endif

#endif

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
    #endif
#endif

namespace BloombergLP {

namespace btlso {

class SocketOptions;

                        // ====================
                        // struct SocketOptUtil
                        // ====================

struct SocketOptUtil {
    // This class provides a namespace for platform independent pure procedures
    // to set and get options for operating-system-level sockets.  These
    // procedures take an 'errorCode' as an optional parameter, which is loaded
    // with platform-specific error number if an error occurs during the
    // operation.  All functions return O on success and a negative value on
    // error.

#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
    struct LingerData {
        // The linger structure maintains information about a specific socket
        // that specifies how that socket should behave when data is queued to
        // be sent and the socket is being closed.

        int  l_onoff;   // Indicate whether to linger or not.  (on = 1/off = 0)
                        // If it is set to a non zero value then 'l_linger'
                        // contains the time in seconds.

        int  l_linger;  // How long to linger for.  If 'l_onoff' is set to
                        // zero then the process returns immediately.
    };
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
    struct LingerData {
        // The linger structure maintains information about a specific socket
        // that specifies how that socket should behave when data is queued to
        // be sent and the socket is being closed.  Note that 'l_linger' is a
        // unsigned short value (max: 65536 seconds).

        u_short  l_onoff;   // On Windows platform 'l_onoff' is defined as
                            // unsigned short.

        u_short  l_linger;  // Enabling SO_LINGER also disables
                            // 'SO_DONTBTESO_LINGER', and vice versa.  Note
                            // that if 'SO_DONTBTESO_LINGER' is disabled (that
                            // is, 'SO_LINGER' is enabled) then no time-out
                            // value is specified.  In this case, the time-out
                            // used is implementation dependent.
    };
#endif

    enum {
        // When manipulating options, the level at which the option resides
        // and the name of the option must be specified.

        k_SOCKETLEVEL     = SOL_SOCKET,     // System socket level
        k_TCPLEVEL        = IPPROTO_TCP     // Protocol level (TCP)


    };

    // For level = k_SOCKETLEVEL
    // The socket options map directly onto the native option values.

    enum {
        k_DEBUGINFO      = SO_DEBUG,      // enable/disable recording of
                                          // debugging information

        k_REUSEADDRESS   = SO_REUSEADDR,  // enable/disable local address reuse

        k_KEEPALIVE      = SO_KEEPALIVE,  // enable/disable keep connections
                                          // alive

        k_DONTROUTE      = SO_DONTROUTE,  // enable/disable routing bypass for
                                          // outgoing messages

        k_LINGER         = SO_LINGER,     // linger on close if data is present

        k_BROADCAST      = SO_BROADCAST,  // enable/disable permission to
                                          // transmit broadcast messages

        k_OOBINLINE      = SO_OOBINLINE,  // enable/disable reception of
                                          // out-of-band data in band

        k_SENDBUFFER     = SO_SNDBUF,     // set buffer size for output

        k_RECEIVEBUFFER  = SO_RCVBUF,     // set buffer size for input

        k_SENDLOWATER    = SO_SNDLOWAT,   // set minimum count for output

        k_RECEIVELOWATER = SO_RCVLOWAT,   // set minimum count for input

        k_SENDTIMEOUT    = SO_SNDTIMEO,   // set timeout value for output

        k_RECEIVETIMEOUT = SO_RCVTIMEO,   // set timeout value for output

        k_TYPE           = SO_TYPE,       // get the type of the socket (get
                                          // only)

        k_SOCKETERROR    = SO_ERROR       // get and clear error on the socket
                                          // (get only)
    };

    // For level = k_TCPLEVEL
    enum {
        k_TCPNODELAY = TCP_NODELAY  // Specifies whether TCP should follow the
                                    // Nagle algorithm for deciding when to
                                    // send data.  By default, TCP will follow
                                    // the Nagle algorithm.  To disable this
                                    // behavior, applications can enable
                                    // TCP_NODELAY to force TCP to always send
                                    // data immediately.


    };

    template <class T>
    static int setOption(SocketHandle::Handle handle,
                         int                  level,
                         int                  option,
                         const T&             value);
        // Set the option of the system socket having the specified 'handle' to
        // the specified 'value' for the specified 'level'.  Return 0, with no
        // effect on 'errorCode', on success.  Otherwise it returns a negative
        // value.

    template <class T>
    static int setOption(SocketHandle::Handle  handle,
                         int                   level,
                         int                   option,
                         const T&              Value,
                         int                  *errorCode);
        // Set the option of the system socket having the specified 'handle' to
        // the specified 'value' for the specified 'level' and load into the
        // specified 'errorCode' the native error code of the operation, if
        // any.  Return 0, with no effect on 'errorCode', on success.
        // Otherwise it returns a negative value.  The behavior is undefined if
        // the specified 'errorCode' is 0.

    template <class T>
    static int getOption(T                    *result,
                         SocketHandle::Handle  handle,
                         int                   level,
                         int                   option);
        // Load into the specified 'result' the value of the specified socket
        // 'option', of the specified 'level', for the socket identified by the
        // specified 'handle'.  Return 0, with no effect on 'errorCode', on
        // success and return a negative value otherwise.  The behavior is
        // undefined if the specified 'result' is 0.

    template <class T>
    static int getOption(T                    *result,
                         SocketHandle::Handle  handle,
                         int                   level,
                         int                   option,
                         int                  *errorCode);
        // Load into the specified 'result' the value of the specified socket
        // 'option', of the specified 'level', for the socket identified by the
        // specified 'handle'.  Return 0, with no effect on 'errorCode', on
        // success.  Otherwise return a negative value and load into the
        // specified 'errorCode' the native error code of the operation.  The
        // behavior is undefined if the specified 'result' is 0.

    static int setSocketOptions(SocketHandle::Handle handle,
                                const SocketOptions& options);
        // Set the specified socket 'options' on the specified 'handle'.
        // Return 0 on success and a non-zero value otherwise.
};

// ============================================================================
//                     INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------
                        // struct SocketOptUtil
                        // --------------------

template <class T>
inline int
SocketOptUtil::setOption(SocketHandle::Handle  handle,
                         int                   level,
                         int                   option,
                         const T&              value,
                         int                  *errorCode)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    if (0 == setsockopt(handle,
                        level,
                        option,
                        reinterpret_cast<char *>(const_cast<T *>(&value)),
                        sizeof value)) {
        return 0;                                                     // RETURN
    }
    else {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }
#else
    if (0 == setsockopt(handle,
                        level,
                        option,
                        static_cast<const void *>(&value),
                        sizeof value)) {
        return 0;                                                     // RETURN
    }
    else {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
#endif
}

template <class T>
inline int
SocketOptUtil::getOption(T                    *result,
                         SocketHandle::Handle  handle,
                         int                   level,
                         int                   option,
                         int                  *errorCode)

{
#if defined (BSLS_PLATFORM_OS_AIX)     ||   \
    defined (BSLS_PLATFORM_OS_LINUX)   ||   \
    defined (BSLS_PLATFORM_OS_FREEBSD) ||   \
    defined (BSLS_PLATFORM_OS_CYGWIN)  ||   \
    defined (BSLS_PLATFORM_OS_DARWIN)  ||   \
    defined (BSLS_PLATFORM_OS_SOLARIS)

    socklen_t optsize;
#else
    int optsize;
#endif

    optsize = sizeof *result;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    if (0 == getsockopt(handle,
                        level,
                        option,
                        static_cast<char *>(const_cast<void *>(
                                            static_cast<const void *>(result))),
                        &optsize)) {
        return 0;                                                     // RETURN
    }
    else {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }
#else
    if (0 == getsockopt(handle,
                        level,
                        option,
                        static_cast<void *>(result),
                        &optsize)) {
        return 0;                                                     // RETURN
    }
    else {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
#endif
}

template <class T>
inline int
SocketOptUtil::setOption(SocketHandle::Handle handle,
                         int                  level,
                         int                  option,
                         const T&             value)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return setsockopt(handle,
                      level,
                      option,
                      (const char*)&value,
                      sizeof value );
#else
    return setsockopt(handle,
                      level,
                      option,
                      (const void*)&value,
                      sizeof value );
#endif
}

template <class T>
inline int
SocketOptUtil::getOption(T                    *result,
                         SocketHandle::Handle  handle,
                         int                   level,
                         int                   option)
{
#if defined (BSLS_PLATFORM_OS_AIX)     ||    \
    defined (BSLS_PLATFORM_OS_LINUX)   ||    \
    defined (BSLS_PLATFORM_OS_FREEBSD) ||    \
    defined (BSLS_PLATFORM_OS_CYGWIN)  ||    \
    defined (BSLS_PLATFORM_OS_DARWIN)  ||    \
    defined (BSLS_PLATFORM_OS_SOLARIS)

    socklen_t optsize;
#else
    int optsize;
#endif
    optsize = sizeof *result;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    return getsockopt(handle,
                      level,
                      option,
                      reinterpret_cast<char *>(result),
                      &optsize);
#else
    return getsockopt(handle,
                      level,
                      option,
                      static_cast<void *>(result),
                      &optsize);
#endif
}

}  // close package namespace

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
