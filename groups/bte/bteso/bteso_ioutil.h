// bteso_ioutil.h                   -*-C++-*-
#ifndef INCLUDED_BTESO_IOUTIL
#define INCLUDED_BTESO_IOUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide operations to set socket I/O modes.
//
//@CLASSES:
//   bteso_IoUtil: namespace for utilities to set socket I/O modes
//
//@AUTHOR: Jianzhong Lu (jlu)
//
//@SEE_ALSO: bteso_socketoptutil bteso_socketimputil
//
//@DESCRIPTION: This component provides a namespace for a collection of
// pure procedures to manipulate I/O modes on sockets.  The modes are
// enumerated for non-platform-specific option classification.  The following
// I/O modes are supported: blocking, non-blocking, close-on-exec, synchronous
// and asynchronous (signal driven I/O).  All procedures return 0 on success
// and a non-zero value on failure.
//
///Usage
//------
// The following snippets of code illustrate how to make a socket non-blocking.
//..
//      bteso_SocketHandle::Handle socketHandle;
//      int nativeErrNo, s;
//      bteso_IoUtil::BlockingMode option = bteso_IoUtil::BTESO_NONBLOCKING;
//      bteso_IoUtil::BlockingMode result;
//      bteso_SocketImpUtil::open<bteso_IPv4Address>(&socketHandle,
//                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
//                                    &nativeErrNo);
//      s = bteso_IoUtil::setBlockingMode(socketHandle,
//                                        option, &nativeErrNo);
//      assert(0 == s);
//      assert(0 == nativeErrNo);
//..
//  Next use 'getBlockingMode' to verify the mode is set correctly:
//..
//      s = bteso_IoUtil::getBlockingMode(&result,
//                                        socketHandle,
//                                        &nativeErrNo);
//      assert(0 == s);
//      assert(0 == nativeErrNo);
//      assert(bteso_IoUtil::BTESO_NONBLOCKING == result);
//..
//  Now set blocking mode on the socket:
//..
//      option = bteso_IoUtil::BTESO_BLOCKING;
//      s = bteso_IoUtil::setBlockingMode(socketHandle, option,
//                                        &nativeErrNo);
//      assert(0 == s);   assert(0 == nativeErrNo);
//..
//  And get blocking mode value again:
//..
//      s = bteso_IoUtil::getBlockingMode(&result,
//                                        socketHandle,
//                                        &nativeErrNo);
//      assert(0 == s);   assert(bteso_IoUtil::BTESO_BLOCKING == result);
//      assert(0 == nativeErrNo);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

                            // ===================
                            // struct bteso_IoUtil
                            // ===================

namespace BloombergLP {

struct bteso_IoUtil {
        // This class provides a namespace for platform-independent pure
        // procedures to set and get the I/O mode on system socket operations.
        // Note that all methods take an 'errorCode' as an optional parameter,
        // which is loaded with a platform-specific error number if
        // an error occurs during the operation.

    enum BlockingMode {
        BTESO_BLOCKING           =  0,   // blocking mode
        BTESO_NONBLOCKING        =  1    // non-blocking mode
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , BLOCKING    = BTESO_BLOCKING
      , NONBLOCKING = BTESO_NONBLOCKING
#endif
    };

    enum AsyncMode {
        BTESO_SYNCHRONOUS      =  0,     // synchronous mode turned on
        BTESO_ASYNCHRONOUS     =  1      // synchronous mode turned off
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SYNCHRONOUS  = BTESO_SYNCHRONOUS
      , ASYNCHRONOUS = BTESO_ASYNCHRONOUS
#endif
    };

    static int setBlockingMode(bteso_SocketHandle::Handle handle,
                               bteso_IoUtil::BlockingMode value,
                               int                       *errorCode = 0);
        // Set the blocking mode of the system socket having the
        // specified 'handle' to the specified 'value', and load into the
        // optionally specified 'errorCode' the native error code for the
        // operation, if any.  Return 0 (with no effect on 'errorCode'), on
        // success and a non-zero value otherwise.

    static int setCloseOnExec(bteso_SocketHandle::Handle handle,
                              int                        value,
                              int                       *errorCode = 0);
        // Set the close-on-exec mode of the system socket having the
        // specified 'handle' to the specified 'value', and load into the
        // optionally specified 'errorCode' the native error code of the
        // operation, if any.  Return 0 (with no effect on 'errorCode'), on
        // success and a non-zero value otherwise.  Note that this I/O mode
        // applies only to UNIX system sockets; a non-zero value with no
        // effect on 'errorCode', is returned on Windows systems.

    static int setAsync(bteso_SocketHandle::Handle handle,
                        bteso_IoUtil::AsyncMode    value,
                        int                       *errorCode = 0);
        // Set the asynchronous mode of the system socket having the
        // specified 'handle' to the specified 'value', and load into the
        // optionally specified 'errorCode' the native error code of the
        // operation, if any.  Return 0 (with no effect on 'errorCode'), on
        // success and a non-zero value otherwise.  Note that this I/O mode
        // applies only to UNIX system sockets; a non-zero value
        // with no effect on 'errorCode', is returned on Windows systems.

    static int getBlockingMode(bteso_IoUtil::BlockingMode  *result,
                               bteso_SocketHandle::Handle   handle,
                               int                         *errorCode = 0);
        // Load into the specified 'result' the blocking mode of the socket
        // having the specified 'handle'.  Return 0 (with no effect on
        // 'errorCode'), on success.  Otherwise, return a non-zero value
        // and load into the optionally specified 'errorCode' the native
        // error code of the operation; a non-zero value (with no
        // effect on 'errorCode'), is returned on Windows systems.  The
        // behavior is undefined if the 'result' is 0.

    static int getCloseOnExec(int                        *result,
                              bteso_SocketHandle::Handle  handle,
                              int                        *errorCode = 0);
        // Load into the specified 'result' the close-on-exec mode of the
        // socket having the specified 'handle'.  Return 0 (with no effect
        // on 'errorCode'), on success.  Otherwise, return a non-zero
        // value and load into the optionally specified 'errorCode'
        // the native error code of the operation.  A non-zero value (with no
        // effect on 'errorCode'), is returned on Windows systems.
        // The behavior is undefined if the 'result' is 0.

    static int getAsync(bteso_IoUtil::AsyncMode     *result,
                        bteso_SocketHandle::Handle   handle,
                        int                         *errorCode = 0);
        // Load into the specified 'result' the asynchronous mode of the
        // socket having the specified 'handle'.  Return 0 (with no effect
        // on 'errorCode'), on success.  Otherwise, return a non-zero
        // value and load into the optionally specified 'errorCode'
        // the native error code of the operator.  A non-zero value (with no
        // effect on 'errorCode'), is returned on Windows systems.
        // The behavior is undefined if the 'result' is 0.
};

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
