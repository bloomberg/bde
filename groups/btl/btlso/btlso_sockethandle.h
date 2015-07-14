// btlso_sockethandle.h            -*-C++-*-
#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#define INCLUDED_BTLSO_SOCKETHANDLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: define a platform independent socket handle type
//
//@CLASSES:
// btlso::SocketHandle: a namespace for socket handle's type definition
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@DESCRIPTION: This component defines a platform independent socket
// handle.
//
///Usage
///-----
//..
//           btlso::SocketHandle::Handle handle;
//..
//

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
    #endif
#endif

namespace BloombergLP {

namespace btlso {
struct SocketHandle {
    // This is a namespace for a typedef for the socket handle
    // which has a platform specific type.

    // TYPES
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    typedef SOCKET Handle;
#else
    typedef int Handle;
#endif

    static const Handle INVALID_SOCKET_HANDLE;
        // Provide a reference specifying an invalid socket.

    enum {
        // These codes represent a non platform specific error classifications.
        // More than one platform specific code may map onto a single error
        // classification.

        BTESO_ERROR_EOF          = -1,
            // The peer closed its send side of the connection.  This includes
            // the normal closure of a connection but does not include when
            // connection is terminated by the peer without a normal closure.
        BTESO_ERROR_UNCLASSIFIED = -2,
            // The platform specific error code could not be mapped onto any
            // error classification.
        BTESO_ERROR_CONNDEAD     = -3,
            // The connection does not represent a connected socket.
        BTESO_ERROR_WOULDBLOCK   = -4,
            // The socket is a non blocking socket and the call would block.
        BTESO_ERROR_NORESOURCES  = -5,
            // Resources were not available to complete the call.
        BTESO_ERROR_INTERRUPTED  = -6,
            // The call was interrupted.  For receive and send calls no data
            // was transferred.
        BTESO_ERROR_TIMEDOUT      = -7
            // A system call timed out.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ERROR_EOF          = BTESO_ERROR_EOF
      , ERROR_UNCLASSIFIED = BTESO_ERROR_UNCLASSIFIED
      , ERROR_CONNDEAD     = BTESO_ERROR_CONNDEAD
      , ERROR_WOULDBLOCK   = BTESO_ERROR_WOULDBLOCK
      , ERROR_NORESOURCES  = BTESO_ERROR_NORESOURCES
      , ERROR_INTERRUPTED  = BTESO_ERROR_INTERRUPTED
      , ERROR_TIMEDOUT     = BTESO_ERROR_TIMEDOUT
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
};
}  // close package namespace

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
