// bteso_sockethandle.h            -*-C++-*-
#ifndef INCLUDED_BTESO_SOCKETHANDLE
#define INCLUDED_BTESO_SOCKETHANDLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: define a platform independent socket handle type
//
//@CLASSES:
// bteso_SocketHandle: a namespace for socket handle's type definition
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@DESCRIPTION: This component defines a platform independent socket
// handle.
//
//USAGE
//-----
//..
//           bteso_SocketHandle::Handle handle;
//..
//

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifdef BTESO_PLATFORM__WIN_SOCKETS
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
    #endif
#endif

namespace BloombergLP {

struct bteso_SocketHandle {
    // This is a namespace for a typedef for the socket handle
    // which has a platform specific type.

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

    // TYPES
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    typedef SOCKET Handle;
#else
    typedef int Handle;
#endif
};

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
