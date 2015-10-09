// btlso_ioutil.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_ioutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_ioutil_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>
#include <bsls_assert.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>
#endif

#ifdef BSLS_PLATFORM_OS_HPUX
#include <stropts.h>
#include <sys/pstat.h>
#endif

#ifdef BSLS_PLATFORM_OS_CYGWIN
#include <asm/socket.h>
#include <sys/ioctl.h>
#endif

namespace BloombergLP {

namespace btlso {

                        // -------------
                        // struct IoUtil
                        // -------------

int IoUtil::setBlockingMode(SocketHandle::Handle  handle,
                            IoUtil::BlockingMode  value,
                            int                  *errorCode)
{

#ifdef BSLS_PLATFORM_OS_WINDOWS

    unsigned long temp = (unsigned long) value;
    if (0 != ioctlsocket(handle, FIONBIO, &temp)) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }
    return 0;

#elif defined(BSLS_PLATFORM_OS_CYGWIN)

    if (0 != ioctl(handle, FIONBIO, (unsigned int *) &value)) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
    return 0;

#else

    int flags;
    if (-1 == (flags = fcntl(handle, F_GETFL, 0))) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    if (e_NONBLOCKING == value) {
        if (-1 == fcntl(handle, F_SETFL, flags | O_NONBLOCK)) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;                                                // RETURN
        }
    }
    else {
        if (-1 == fcntl(handle, F_SETFL, (flags & ~O_NONBLOCK))) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;                                                // RETURN
        }
    }
    return 0;

#endif
}

int IoUtil::setCloseOnExec(SocketHandle::Handle  handle,
                           int                   value,
                           int                  *errorCode)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS

    return -1;          // not available

#else

    if (-1 == fcntl(handle,F_SETFD, !!value)) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    return 0;
#endif
}

int IoUtil::setAsync(SocketHandle::Handle  handle,
                     IoUtil::AsyncMode     value,
                     int                  *errorCode)
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)

    // 'fcntl' does not work for FASYNC on Cygwin.  'ioctl' could work but
    // Cygwin does not have a pstat facility.

    return -1;          // not available

#elif defined(BSLS_PLATFORM_OS_HPUX) || defined(BSLS_PLATFORM_OS_CYGWIN)

    // HPUX does not support setting ASYNC flag through 'fcntl'.  Therefore, we
    // must 'ioctl' with 'FIOASYNC' flag.

    int flag = (e_ASYNCHRONOUS == value) ? 1 : 0;

    if (e_ASYNCHRONOUS == value && -1 == fcntl(handle, F_SETOWN, getpid())) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
    if (ioctl(handle, FIOASYNC, &flag)) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
    return 0;

#else

    int flags;
    if (-1 == (flags = fcntl(handle, F_GETFL, 0))) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    if (e_ASYNCHRONOUS == value) {
        if (-1 == fcntl(handle, F_SETOWN, getpid())) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;                                                // RETURN
        }
        if (-1 == fcntl(handle, F_SETFL, flags | FASYNC)) {
                                              // ^^^^^^ O_ASYNC in posix
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;                                                // RETURN
        }
    }
    else {
        if (-1 == fcntl(handle, F_SETFL, (flags & ~FASYNC))) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;                                                // RETURN
        }
    }
    return 0;

#endif
}

int IoUtil::getBlockingMode(IoUtil::BlockingMode *result,
                            SocketHandle::Handle  handle,
                            int                  *errorCode)
{
    BSLS_ASSERT(result);

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)

        return -1;         // not available

#else

    int flags;
    if (-1 == (flags = fcntl(handle, F_GETFL, 0))) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    *result = 0 == (flags & O_NONBLOCK) ? e_BLOCKING : e_NONBLOCKING;
    return 0;

#endif
}

int IoUtil::getCloseOnExec(int                  *result,
                           SocketHandle::Handle  handle,
                           int                  *errorCode)
{
    BSLS_ASSERT(result);

#if defined(BSLS_PLATFORM_OS_WINDOWS)

    return -1;          // not available

#else

    if(-1 == (*result=fcntl(handle, F_GETFD, 0))) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
    return 0;

#endif
}

int IoUtil::getAsync(IoUtil::AsyncMode    *result,
                     SocketHandle::Handle  handle,
                     int                  *errorCode)
{
    BSLS_ASSERT(result);

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)

    return -1;          // not available

#elif defined(BSLS_PLATFORM_OS_HPUX)

    struct pst_fileinfo2 f_info;
    struct pst_socket    s_info;

    int retval = pstat_getfile2(&f_info,
                                sizeof(f_info),
                                0,
                                (int)handle,
                                getpid());

    if (retval != 1) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    retval = pstat_getsocket(&s_info,
                             sizeof(s_info),
                             &(f_info.psf_fid));

    if (retval != 1) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    *result = 0 == (s_info.pst_state & PS_SS_ASYNC)
              ? e_SYNCHRONOUS
              : e_ASYNCHRONOUS;
    return 0;

#else

    int flags;
    if (-1 == (flags = fcntl(handle, F_GETFL, 0))) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }
    *result = (flags & FASYNC) == 0 ? e_SYNCHRONOUS : e_ASYNCHRONOUS;
    return 0;

#endif
}

}  // close package namespace

}  // close enterprise namespace

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
