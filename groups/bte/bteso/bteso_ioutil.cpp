// bteso_ioutil.cpp                 -*-C++-*-
#include <bteso_ioutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_ioutil_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_ipv4address.h>                  // for testing only
#include <bteso_socketimputil.h>                // for testing only
#endif

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

                        // ===================
                        // struct bteso_IoUtil
                        // ===================

namespace BloombergLP {

int bteso_IoUtil::setBlockingMode(bteso_SocketHandle::Handle handle,
                                  bteso_IoUtil::BlockingMode value,
                                  int                       *errorCode)
{
    #ifdef BSLS_PLATFORM_OS_WINDOWS
        unsigned long temp = (unsigned long) value;
        if (ioctlsocket(handle, FIONBIO, &temp) != 0) {
            if (errorCode) {
                *errorCode = WSAGetLastError();
            }
            return -1;
        }
        return 0;
    #elif defined(BSLS_PLATFORM_OS_CYGWIN)
        if (ioctl(handle, FIONBIO, (unsigned int *) &value) != 0) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        return 0;
    #else
        int flags;
        if ((flags = fcntl(handle, F_GETFL, 0)) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        if (BTESO_NONBLOCKING == value) {
            if (fcntl(handle, F_SETFL, flags | O_NONBLOCK) == -1) {
                if (errorCode) {
                    *errorCode = errno;
                }
                return -1;
            }
        }
        else {
            if (fcntl(handle, F_SETFL, (flags & ~O_NONBLOCK)) == -1) {
                if (errorCode) {
                    *errorCode = errno;
                }
                return -1;
            }
       }
       return 0;
    #endif
}

int bteso_IoUtil::setCloseOnExec(bteso_SocketHandle::Handle handle,
                                 int                        value,
                                 int                       *errorCode)
{
    #ifdef BSLS_PLATFORM_OS_WINDOWS
        return -1;          // not available
    #else
        if (fcntl(handle,F_SETFD, !!value) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
    #endif
    return 0;
}

int bteso_IoUtil::setAsync(bteso_SocketHandle::Handle handle,
                           bteso_IoUtil::AsyncMode    value,
                           int                       *errorCode)
{
    #if defined(BSLS_PLATFORM_OS_WINDOWS)

        // 'fcntl' does not work for FASYNC on Cygwin.  'ioctl' could work
        // but Cygwin does not have a pstat facility.

        return -1;          // not available
    #elif defined(BSLS_PLATFORM_OS_HPUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
        // HPUX does not support setting ASYNC flag through 'fcntl'.
        // Therefore, we must 'ioctl' with 'FIOASYNC' flag.
        int flag = (BTESO_ASYNCHRONOUS == value) ? 1 : 0;

        if ( BTESO_ASYNCHRONOUS == value &&
             fcntl(handle, F_SETOWN, getpid()) == -1) {
              if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        if (ioctl(handle, FIOASYNC, &flag)) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        return 0;
    #else
        int flags;
        if ((flags = fcntl(handle, F_GETFL, 0)) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        if (BTESO_ASYNCHRONOUS == value) {
            if (fcntl(handle, F_SETOWN, getpid()) == -1) {
                if (errorCode) {
                    *errorCode = errno;
                }
                return -1;
            }
            if (fcntl(handle, F_SETFL, flags | FASYNC) == -1) {
                                            // ^^^^^^ O_ASYNC in posix
                if (errorCode) {
                    *errorCode = errno;
                }
                return -1;
            }

        }
        else {
            if (fcntl(handle, F_SETFL, (flags & ~FASYNC)) == -1) {
                if (errorCode) {
                    *errorCode = errno;
                }
                return -1;
             }
        }
        return 0;
    #endif
}

int bteso_IoUtil::getBlockingMode(bteso_IoUtil::BlockingMode  *result,
                                  bteso_SocketHandle::Handle   handle,
                                  int                         *errorCode)
{
    BSLS_ASSERT(result);
    #if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
        return -1;         // not available
    #else
        int flags;
        if ((flags = fcntl(handle, F_GETFL, 0)) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        *result = (flags & O_NONBLOCK) == 0
                  ? BTESO_BLOCKING
                  : BTESO_NONBLOCKING;
        return 0;
    #endif
}

int bteso_IoUtil::getCloseOnExec(int                        *result,
                                 bteso_SocketHandle::Handle  handle,
                                 int                        *errorCode)
{
    BSLS_ASSERT(result);
    #if defined(BSLS_PLATFORM_OS_WINDOWS)
        return -1;          // not available
    #else
        if((*result=fcntl(handle, F_GETFD, 0)) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        return 0;
    #endif
}

int bteso_IoUtil::getAsync(bteso_IoUtil::AsyncMode   *result,
                           bteso_SocketHandle::Handle handle,
                           int                       *errorCode)
{
    BSLS_ASSERT(result);
    #if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
        return -1;          // not available
    #elif defined(BSLS_PLATFORM_OS_HPUX)
        struct pst_fileinfo2 f_info;
        struct pst_socket    s_info;

        int retval = pstat_getfile2 ( &f_info, sizeof(f_info),
                                      0, (int)handle, getpid() );
        if (retval != 1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }

        retval = pstat_getsocket ( &s_info, sizeof(s_info),
                                   &(f_info.psf_fid ) );
        if (retval != 1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }

        *result = (s_info.pst_state & PS_SS_ASYNC) == 0 ?
             BTESO_SYNCHRONOUS : BTESO_ASYNCHRONOUS;
        return 0;
    #else
        int flags;
        if ((flags = fcntl(handle, F_GETFL, 0)) == -1) {
            if (errorCode) {
                *errorCode = errno;
            }
            return -1;
        }
        *result = (flags & FASYNC) == 0
                  ? BTESO_SYNCHRONOUS
                  : BTESO_ASYNCHRONOUS;
        return 0;
    #endif
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
