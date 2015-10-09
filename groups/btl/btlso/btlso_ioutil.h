// btlso_ioutil.h                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_IOUTIL
#define INCLUDED_BTLSO_IOUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide operations to set socket I/O modes.
//
//@CLASSES:
//   btlso::IoUtil: namespace for utilities to set socket I/O modes
//
//@SEE_ALSO: btlso_socketoptutil btlso_socketimputil
//
//@DESCRIPTION: This component provides a namespace for a collection of pure
// procedures to manipulate I/O modes on sockets.  The modes are enumerated for
// non-platform-specific option classification.  The following I/O modes are
// supported: blocking, non-blocking, close-on-exec, synchronous and
// asynchronous (signal driven I/O).  All procedures return 0 on success and a
// non-zero value on failure.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Changing the Blocking Mode on a Socket
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to make a socket non-blocking.
//..
//  btlso::SocketHandle::Handle socketHandle;
//  int                         nativeErrNo, s;
//  btlso::IoUtil::BlockingMode option = btlso::IoUtil::e_NONBLOCKING;
//  btlso::IoUtil::BlockingMode result;
//
//  btlso::SocketImpUtil::open<btlso::IPv4Address>(
//                                       &socketHandle,
//                                       btlso::SocketImpUtil::k_SOCKET_STREAM,
//                                       &nativeErrNo);
//  s = btlso::IoUtil::setBlockingMode(socketHandle, option, &nativeErrNo);
//  assert(0 == s);
//  assert(0 == nativeErrNo);
//..
//  Next use 'getBlockingMode' to verify the mode is set correctly:
//..
//  s = btlso::IoUtil::getBlockingMode(&result, socketHandle, &nativeErrNo);
//
//  assert(0 == s);
//  assert(0 == nativeErrNo);
//  assert(btlso::IoUtil::e_NONBLOCKING == result);
//..
//  Now set blocking mode on the socket:
//..
//  option = btlso::IoUtil::e_BLOCKING;
//  s = btlso::IoUtil::setBlockingMode(socketHandle, option, &nativeErrNo);
//
//  assert(0 == s);   assert(0 == nativeErrNo);
//..
//  And get blocking mode value again:
//..
//  s = btlso::IoUtil::getBlockingMode(&result, socketHandle, &nativeErrNo);
//
//  assert(0 == s);   assert(btlso::IoUtil::e_BLOCKING == result);
//  assert(0 == nativeErrNo);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

namespace BloombergLP {

namespace btlso {

                            // =============
                            // struct IoUtil
                            // =============

struct IoUtil {
    // This class provides a namespace for platform-independent pure procedures
    // to set and get the I/O mode on system socket operations.  Note that all
    // methods take an 'errorCode' as an optional parameter, which is loaded
    // with a platform-specific error number if an error occurs during the
    // operation.

    enum BlockingMode {
        e_BLOCKING    = 0,   // blocking mode
        e_NONBLOCKING = 1    // non-blocking mode


    };

    enum AsyncMode {
        e_SYNCHRONOUS  = 0,     // synchronous mode turned on
        e_ASYNCHRONOUS = 1      // synchronous mode turned off


    };

    static int setBlockingMode(SocketHandle::Handle  handle,
                               IoUtil::BlockingMode  value,
                               int                  *errorCode = 0);
        // Set the blocking mode of the system socket having the specified
        // 'handle' to the specified 'value', and load into the optionally
        // specified 'errorCode' the native error code for the operation, if
        // any.  Return 0 (with no effect on 'errorCode'), on success and a
        // non-zero value otherwise.

    static int setCloseOnExec(SocketHandle::Handle  handle,
                              int                   value,
                              int                  *errorCode = 0);
        // Set the close-on-exec mode of the system socket having the specified
        // 'handle' to the specified 'value', and load into the optionally
        // specified 'errorCode' the native error code of the operation, if
        // any.  Return 0 (with no effect on 'errorCode'), on success and a
        // non-zero value otherwise.  Note that this I/O mode applies only to
        // UNIX system sockets; a non-zero value with no effect on 'errorCode',
        // is returned on Windows systems.

    static int setAsync(SocketHandle::Handle  handle,
                        IoUtil::AsyncMode     value,
                        int                  *errorCode = 0);
        // Set the asynchronous mode of the system socket having the specified
        // 'handle' to the specified 'value', and load into the optionally
        // specified 'errorCode' the native error code of the operation, if
        // any.  Return 0 (with no effect on 'errorCode'), on success and a
        // non-zero value otherwise.  Note that this I/O mode applies only to
        // UNIX system sockets; a non-zero value with no effect on 'errorCode',
        // is returned on Windows systems.

    static int getBlockingMode(IoUtil::BlockingMode *result,
                               SocketHandle::Handle  handle,
                               int                  *errorCode = 0);
        // Load into the specified 'result' the blocking mode of the socket
        // having the specified 'handle'.  Return 0 (with no effect on
        // 'errorCode'), on success.  Otherwise, return a non-zero value and
        // load into the optionally specified 'errorCode' the native error code
        // of the operation; a non-zero value (with no effect on 'errorCode'),
        // is returned on Windows systems.  The behavior is undefined if the
        // 'result' is 0.

    static int getCloseOnExec(int                  *result,
                              SocketHandle::Handle  handle,
                              int                  *errorCode = 0);
        // Load into the specified 'result' the close-on-exec mode of the
        // socket having the specified 'handle'.  Return 0 (with no effect on
        // 'errorCode'), on success.  Otherwise, return a non-zero value and
        // load into the optionally specified 'errorCode' the native error code
        // of the operation.  A non-zero value (with no effect on 'errorCode'),
        // is returned on Windows systems.  The behavior is undefined if the
        // 'result' is 0.

    static int getAsync(IoUtil::AsyncMode    *result,
                        SocketHandle::Handle  handle,
                        int                  *errorCode = 0);
        // Load into the specified 'result' the asynchronous mode of the socket
        // having the specified 'handle'.  Return 0 (with no effect on
        // 'errorCode'), on success.  Otherwise, return a non-zero value and
        // load into the optionally specified 'errorCode' the native error code
        // of the operator.  A non-zero value (with no effect on 'errorCode'),
        // is returned on Windows systems.  The behavior is undefined if the
        // 'result' is 0.
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
