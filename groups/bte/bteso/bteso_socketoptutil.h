// bteso_socketoptutil.h           -*-C++-*-
#ifndef INCLUDED_BTESO_SOCKETOPTUTIL
#define INCLUDED_BTESO_SOCKETOPTUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide operations to manipulate socket options.
//
//@CLASSES:
//   bteso_SocketOptUtil: namespace for socket options utilities
//
//@AUTHOR: Khalid Shafiq (kshafiq)
//
//@SEE_ALSO: bteso_socketimputil bteso_ioutil
//
//@DESCRIPTION: This component provides a namespace for a collection of
// pure procedures to manipulate options on sockets.  These options are
// enumerated for non-platform-specific option classifications.  These options
// can exist at multiple levels such as 'SOL_SOCKET', 'IPPROTO_TCP' and
// 'IPPROTO_IP'.  The supported levels include 'SOL_SOCKET' and 'IPPROTO_TCP'.
// This component acts as a pass-through between the system and the
// application.  Particularly, the option name and option level are passed
// directly (i.e., without any modification) into system calls.
//
///Usage
///-----
///Usage 1
///- - - -
// The following snippets of code illustrate how to set 'BTESO_REUSEADDRESS'
// flag on a socket.  Note that we assume that a socket of type 'SOCK_STREAM'
// is already created and socket allocation 'bteso_socketimputil' can be used.
//..
//      int result;
//      int addropt = 1;
//      result = bteso_SocketOptUtil::setOption(sockethandle,
//                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                   bteso_SocketOptUtil::BTESO_REUSEADDRESS,
//                   addropt);
//
//      assert(0 == result);
//..
// Now we will verify that the address option was set correctly:
//..
//      addropt = 0;
//      result = bteso_SocketOptUtil::getOption(&addropt, sockethandle,
//                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                   bteso_SocketOptUtil::BTESO_REUSEADDRESS);
//
//      assert(0 == result);
//      assert(0 != addropt);
//..
// Next use 'getOption' to verify the type of the socket ('SOCK_STREAM'):
//..
//      int sockettype = 0;
//      result = bteso_SocketOptUtil::getOption(&sockettype,
//                   sockethandle, bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                   bteso_SocketOptUtil::BTESO_TYPE);
//
//      assert(0 == result);
//      assert(SOCK_STREAM == sockettype);
//..
///Usage 2
///- - - -
// The following snippets of code illustrate how to set and get linger data
// option values for a socket.
// First set the linger interval to 2 seconds:
//..
//      int errorcode = 0;
//      bteso_SocketOptUtil::LingerData ld;
//      ld.l_onoff  = 1;   // Enable lingering for
//      ld.l_linger = 2;   // 2 seconds
//      result = bteso_SocketOptUtil::setOption(sockethandle,
//                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL
//                   bteso_SocketOptUtil::BTESO_LINGER,
//                   ld, &errorcode);
//      assert(0 == result);
//      assert(0 == errorcode);
//..
// Now query that the option was actually set..
//..
//      bteso_SocketOptUtil::LingerData ld2
//      result = bteso_SocketOptUtil::getOption(&ld2, sockethandle,
//                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                   bteso_SocketOptUtil::BTESO_LINGER, &errorcode);
//
//      assert(0 == result);
//      assert(0 == errorcode);
//..
///Usage 3
///- - - -
// The following snippets of code illustrate the pass-through ability.
//..
//  #ifdef BSLS_PLATFORM__OS_WINDOWS
//
//      WSAPROTOCOL_INFO protoInfo;
//      memset(&ProtoInfo,0,sizeof(ProtoInfo));
//      result = bteso_SocketOptUtil::getOption(&protoInfo, sockethandle,
//                   bteso_SocketOptUtil::BTESO_SOCKETLEVEL, SO_PROTOCOL_INFO,
//                   &errorcode);
//
//      assert(0 == result);
//      assert(0 == errorcode);
//      assert(SOCK_STREAM == ProtoInfo.iAddressFamily);
//      assert(ProtocolInformation == ProtoInfo.iProtocol);
//      assert(ProtocolVersion == ProtoInfo.iVersion);
//      assert(SocketType == ProtoInfo.iSocketType);
//
//  #endif
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM__OS_UNIX

    #ifdef BSLS_PLATFORM__OS_AIX
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

#ifdef BTESO_PLATFORM__WIN_SOCKETS
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
    #endif
#endif

                        // ==========================
                        // struct bteso_SocketOptUtil
                        // ==========================

namespace BloombergLP {

struct bteso_SocketOptUtil {
    // This class provides a namespace for platform independent pure procedures
    // to set and get options for operating-system-level sockets.  These
    // procedures take an 'errorCode' as an optional parameter, which is loaded
    // with platform-specific error number if an error occurs during the
    // operation.  All functions return O on success and a negative value on
    // error.

#if defined(BSLS_PLATFORM__OS_UNIX) && !defined(BSLS_PLATFORM__OS_CYGWIN)
    struct LingerData {
        // The linger structure maintains information about a specific socket
        // that specifies how that socket should behave when data is queued to
        // be sent and the socket is being closed.
        int  l_onoff;
             // Indicate whether to linger or not.  (on = 1/off = 0).  If it is
             // set to a non zero value then 'l_linger' contains the time in
             // seconds.
        int  l_linger;
             // How long to linger for.  If 'l_onoff' is set to zero then the
             // process returns immediately.
    };
#endif

#if defined(BSLS_PLATFORM__OS_WINDOWS) || defined(BSLS_PLATFORM__OS_CYGWIN)
    struct LingerData {
        // The linger structure maintains information about a specific socket
        // that specifies how that socket should behave when data is queued to
        // be sent and the socket is being closed.  Note that 'l_linger' is a
        // unsigned short value (max: 65536 seconds).
        u_short  l_onoff;
             // On Windows platform 'l_onoff' is defined as unsigned short.
        u_short  l_linger;
             // Enabling SO_LINGER also disables SO_DONTBTESO_LINGER, and vice
             // versa.  Note that if SO_DONTBTESO_LINGER is disabled (that is,
             // SO_LINGER is enabled) then no time-out value is specified.  In
             // this case, the time-out used is implementation dependent.
    };
#endif

    enum {
        // When manipulating options, the level at which the option resides
        // and the name of the option must be specified.
        BTESO_SOCKETLEVEL    = SOL_SOCKET,    // System socket level
        BTESO_TCPLEVEL       = IPPROTO_TCP    // Protocol level (TCP)
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SOCKETLEVEL = BTESO_SOCKETLEVEL
      , TCPLEVEL    = BTESO_TCPLEVEL
#endif
    };

    // For level = BTESO_SOCKETLEVEL
    // The socket options map directly onto the native option values.
    enum {
        BTESO_DEBUGINFO      = SO_DEBUG,
            // enable/disable recording of debugging information
        BTESO_REUSEADDRESS   = SO_REUSEADDR,
            // enable/disable local address reuse
        BTESO_KEEPALIVE      = SO_KEEPALIVE,
            // enable/disable keep connections alive
        BTESO_DONTROUTE      = SO_DONTROUTE,
            // enable/disable routing bypass for outgoing messages
        BTESO_LINGER         = SO_LINGER,
            // linger on close if data is present
        BTESO_BROADCAST      = SO_BROADCAST,
            // enable/disable permission to transmit broadcast messages
        BTESO_OOBINLINE      = SO_OOBINLINE,
            // enable/disable reception of out-of-band data in band
        BTESO_SENDBUFFER     = SO_SNDBUF,
            // set buffer size for output
        BTESO_RECEIVEBUFFER  = SO_RCVBUF,
            // set buffer size for input
        BTESO_SENDLOWATER    = SO_SNDLOWAT,
            // set minimum count for output
        BTESO_RECEIVELOWATER = SO_RCVLOWAT,
            // set minimum count for input
        BTESO_SENDTIMEOUT    = SO_SNDTIMEO,
            // set timeout value for output
        BTESO_RECEIVETIMEOUT = SO_RCVTIMEO,
            // set timeout value for output
        BTESO_TYPE           = SO_TYPE,
            // get the type of the socket (get only)
        BTESO_SOCKETERROR    = SO_ERROR
            // get and clear error on the socket (get only)
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , DEBUGINFO      = BTESO_DEBUGINFO
      , REUSEADDRESS   = BTESO_REUSEADDRESS
      , KEEPALIVE      = BTESO_KEEPALIVE
      , DONTROUTE      = BTESO_DONTROUTE
      , LINGER         = BTESO_LINGER
      , BROADCAST      = BTESO_BROADCAST
      , OOBINLINE      = BTESO_OOBINLINE
      , SENDBUFFER     = BTESO_SENDBUFFER
      , RECEIVEBUFFER  = BTESO_RECEIVEBUFFER
      , SENDLOWATER    = BTESO_SENDLOWATER
      , RECEIVELOWATER = BTESO_RECEIVELOWATER
      , SENDTIMEOUT    = BTESO_SENDTIMEOUT
      , RECEIVETIMEOUT = BTESO_RECEIVETIMEOUT
      , TYPE           = BTESO_TYPE
      , SOCKETERROR    = BTESO_SOCKETERROR
#endif
    };

    // For level = BTESO_TCPLEVEL
    enum {
        BTESO_TCPNODELAY     = TCP_NODELAY
        // Specifies whether TCP should follow the Nagle algorithm
        // for deciding when to send data.  By default, TCP will follow
        // the Nagle algorithm.  To disable this behavior, applications
        // can enable TCP_NODELAY to force TCP to always send data
        // immediately.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , TCPNODELAY = BTESO_TCPNODELAY
#endif
    };

    template<class T>
    static int setOption(bteso_SocketHandle::Handle handle, int level,
                         int option, const T& Value,
                         int *errorCode);
        // Set the option of the system socket having the specified
        // 'handle' to the specified 'value' for the specified 'level' and
        // load into the specified 'errorCode' the native error code of the
        // operation, if any.  Return 0, with no effect on 'errorCode', on
        // success.  Otherwise it returns a negative value.  The behavior
        // is undefined if the specified 'errorCode' is 0.

    template<class T>
    static int getOption(T *result, bteso_SocketHandle::Handle handle,
                         int level, int option,  int *errorCode);
        // Load into the specified 'result' the value of the specified
        // socket 'option', of the specified 'level', for the socket
        // identified by the specified 'handle'.  Return 0, with no effect
        // on 'errorCode', on success.  Otherwise return a negative value and
        // load into the specified 'errorCode' the native error code of
        // the operation.  The behavior is undefined if the specified
        // 'result' is 0.

    template<class T>
    static int setOption(bteso_SocketHandle::Handle handle, int level,
                         int option, const T& value);
        // Set the option of the system socket having the specified
        // 'handle' to the specified 'value' for the specified 'level'.
        // Return 0, with no effect on 'errorCode', on success.  Otherwise
        // it returns a negative value.

    template<class T>
    static int getOption(T *result, bteso_SocketHandle::Handle handle,
                         int level, int option);
        // Load into the specified 'result' the value of the specified
        // socket 'option', of the specified 'level', for the socket
        // identified by the specified 'handle'.  Return 0, with no effect
        // on 'errorCode', on success and return a negative value otherwise.
        // The behavior is undefined if the specified 'result' is 0.
};

// ============================================================================
//                     INLINE FUNCTION DEFINITIONS
// ============================================================================

template<class T>
inline int
bteso_SocketOptUtil::setOption(bteso_SocketHandle::Handle handle, int level,
                               int option, const T& value,
                               int *errorCode)
{
    #ifdef BSLS_PLATFORM__OS_WINDOWS
    if (0 == setsockopt(handle, level, option,
                        (char*)&value, sizeof value )) {
        return 0;
    }
    else {
        if (errorCode)
            *errorCode = WSAGetLastError();
        return -1;
    }
    #else
    if (0 == setsockopt(handle, level, option,
                        (void*)&value, sizeof value )) {
        return 0;
    }
    else {
        if (errorCode)
            *errorCode = errno;
        return -1;
    }
    #endif
}

template<class T>
inline int
bteso_SocketOptUtil::getOption(T *result, bteso_SocketHandle::Handle handle,
                               int level, int option, int *errorCode)

{
    #if defined (BSLS_PLATFORM__OS_AIX) || defined (BSLS_PLATFORM__CMP_GNU)
        socklen_t optsize;
    #else
        int optsize;
    #endif
    optsize = sizeof *result;

    #ifdef BSLS_PLATFORM__OS_WINDOWS
        if (0 == getsockopt(handle, level, option,
                            (char*)result, &optsize)) {
            return 0;
        }
        else {
            if (errorCode)
                *errorCode = WSAGetLastError();
            return -1;
        }
    #else
        if (0 == getsockopt(handle, level, option,
                            (void*)result, &optsize)) {
            return 0;
        }
        else {
            if (errorCode)
                *errorCode = errno;
            return -1;
        }
    #endif
}

template<class T>
inline int
bteso_SocketOptUtil::setOption(bteso_SocketHandle::Handle handle, int level,
                               int option, const T& value)
{
    #ifdef BSLS_PLATFORM__OS_WINDOWS
        return setsockopt(handle, level, option,
                          (const char*)&value, sizeof value );
    #else
       return setsockopt(handle, level, option,
                         (const void*)&value, sizeof value );
    #endif
}

template<class T>
inline int
bteso_SocketOptUtil::getOption(T *result, bteso_SocketHandle::Handle handle,
                               int level, int option)
{
     #if defined (BSLS_PLATFORM__OS_AIX) ||   \
         defined (BSLS_PLATFORM__OS_LINUX) || \
         defined (BDES_PLATFORM__OS_FREEBSD) || \
         defined (BSLS_PLATFORM__OS_CYGWIN) || \
         defined (BSLS_PLATFORM__OS_SOLARIS)
          socklen_t optsize;
      #else
          int optsize;
      #endif
      optsize = sizeof *result;

      #ifdef BSLS_PLATFORM__OS_WINDOWS
          return getsockopt(handle, level, option,
                            (char*)result, &optsize);
      #else
          return getsockopt(handle, level, option,
                            (void*)result, &optsize);
      #endif
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
