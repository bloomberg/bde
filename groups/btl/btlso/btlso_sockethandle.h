// btlso_sockethandle.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#define INCLUDED_BTLSO_SOCKETHANDLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent socket handle type
//
//@CLASSES:
//  btlso::SocketHandle: a namespace for socket handle's type definition
//
//@DESCRIPTION: This component provides a 'struct', 'btlso::SocketHandle',
// that defines a typedef, 'Handle' for a platform-independent type for
// representing a socket handle.  A unique handle value
// 'INVALID_SOCKET_HANDLE' is provided to denote an invalid socket handle.
// Finally, this component enumerates the list of errors encountered while
// performing socket operations.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Socket Class
/// - - - - - - - - - - - - - - - - - - -
// Writing portable software involves providing a generic interface while
// specializing implementations to work with platform-provided interfaces.  As
// an example, we provide a platform-independent socket 'class', 'my_Socket'
// below.  The 'my_Socket' 'class' provides functionality to establish a
// connection and to read and write data on it.  Internally, the 'class' uses a
// 'btlso::SocketHandle::Handle' object to store the socket handle.  Note that
// for brevity only portions of this 'class' that are relevant to this example
// are shown:
//..
//  class my_Socket {
//      // This 'class' provides a platform-independent socket class.
//
//      // DATA
//      btlso::SocketHandle::Handle d_handle;
//
//      // CREATORS
//      my_Socket();
//          // Create an unconnected 'my_Socket' object.
//
//      // Rest of the interface elided for brevity
//  };
//..
// The default-constructor can then be implemented as follows:
//..
//  my_Socket::my_Socket()
//  : d_handle(btlso::SocketHandle::INVALID_SOCKET_HANDLE)
//  {
//  }
//..

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

                        // ==================
                        // class SocketHandle
                        // ==================

struct SocketHandle {
    // This is a namespace for a typedef for the socket handle which has a
    // platform specific type.

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

        e_ERROR_EOF          = -1,  // The peer closed its send side of the
                                    // connection.  This includes the normal
                                    // closure of a connection but does not
                                    // include when connection is terminated by
                                    // the peer without a normal closure.

        e_ERROR_UNCLASSIFIED = -2,  // The platform specific error code could
                                    // not be mapped onto any error
                                    // classification.

        e_ERROR_CONNDEAD     = -3,  // The connection does not represent a
                                    // connected socket.

        e_ERROR_WOULDBLOCK   = -4,  // The socket is a non blocking socket and
                                    // the call would block.

        e_ERROR_NORESOURCES  = -5,  // Resources were not available to complete
                                    // the call.

        e_ERROR_INTERRUPTED  = -6,  // The call was interrupted.  For receive
                                    // and send calls no data was transferred.

        e_ERROR_TIMEDOUT     = -7   // A system call timed out.


    };
};

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
