// btlso_socketoptutil.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_socketoptutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_socketoptutil_cpp,"$Id$ $CSID$")

#include <btlso_socketoptions.h>
#include <btlso_lingeroptions.h>

namespace BloombergLP {

namespace btlso {

                        // -------------------
                        // class SocketOptUtil
                        // -------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS

// PRIVATE CLASS METHODS
int SocketOptUtil::setReuseAddressOnWindows(
                                        btlso::SocketHandle::Handle  handle,
                                        int                          level,
                                        const char                  *value,
                                        int                          valueLen,
                                        int                         *errorCode)
{
    // Earlier versions of Windows (prior to Windows Server 2003) allowed
    // reusing the port number bound to a socket handle even if that handle
    // set its 'SO_REUSEADDR' option value to 0.  This allowed subsequent
    // applications to "hijack" ports bound to earlier applications.  To
    // fix this security hole, Windows introduced another socket option,
    // 'SO_EXCLUSIVEADDRUSE', that guaranteed that a port bound to an
    // application could not be reused.
    //
    // Even though the 'SO_REUSEADDR' behavior was fixed in Windows starting
    // from Windows Server 2003, it is still recommended to set the
    // 'SO_EXCLUSIVEADDRUSE' option to guarantee that ports are not reused.
    //
    // For more information see:
    //..
    // https://msdn.microsoft.com/en-us/library/windows/
    //                                          desktop/ms740621(v=vs.85).aspx.
    //..
    // In this method we check the value of the 'SO_REUSEADDR' option value and
    // set both the 'SO_REUSEADDR' and 'SO_EXCLUSIVEADDRUSE' options
    // accordingly.

    int exclusiveAddrOptionValue =
                            0 == *reinterpret_cast<const int *>(value) ? 1 : 0;

    int rc = setsockopt(
                     handle,
                     level,
                     SO_EXCLUSIVEADDRUSE,
                     reinterpret_cast<const char *>(&exclusiveAddrOptionValue),
                     sizeof exclusiveAddrOptionValue);
    if (0 != rc) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }

    rc = setsockopt(handle, level, k_REUSEADDRESS, value, valueLen);

    if (0 != rc) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }
    return 0;
}

#endif

// CLASS METHODS
int SocketOptUtil::setSocketOptions(SocketHandle::Handle handle,
                                    const SocketOptions& options)
{
    if (!options.debugFlag().isNull()) {
        const int rc = setOption(
                                handle,
                                SocketOptUtil::k_SOCKETLEVEL,
                                SocketOptUtil::k_DEBUGINFO,
                                static_cast<int>(options.debugFlag().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.allowBroadcasting().isNull()) {
        const int rc = setOption(
                        handle,
                        SocketOptUtil::k_SOCKETLEVEL,
                        SocketOptUtil::k_BROADCAST,
                        static_cast<int>(options.allowBroadcasting().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.reuseAddress().isNull()) {
        const int rc = setOption(
                             handle,
                             SocketOptUtil::k_SOCKETLEVEL,
                             SocketOptUtil::k_REUSEADDRESS,
                             static_cast<int>(options.reuseAddress().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.keepAlive().isNull()) {
        const int rc = setOption(
                                handle,
                                SocketOptUtil::k_SOCKETLEVEL,
                                SocketOptUtil::k_KEEPALIVE,
                                static_cast<int>(options.keepAlive().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.bypassNormalRouting().isNull()) {
        const int rc = setOption(
                      handle,
                      SocketOptUtil::k_SOCKETLEVEL,
                      SocketOptUtil::k_DONTROUTE,
                      static_cast<int>(options.bypassNormalRouting().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.linger().isNull()) {
        const LingerOptions&      lingerOptions = options.linger().value();
        SocketOptUtil::LingerData lingerData;

        lingerData.l_onoff  = lingerOptions.lingerFlag();
        lingerData.l_linger = lingerOptions.timeout();

        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_LINGER,
                                 lingerData);
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        const int rc = setOption(
                 handle,
                 SocketOptUtil::k_SOCKETLEVEL,
                 SocketOptUtil::k_OOBINLINE,
                 static_cast<int>(options.leaveOutOfBandDataInline().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.sendBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_SENDBUFFER,
                                 options.sendBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.receiveBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_RECEIVEBUFFER,
                                 options.receiveBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.minimumSendBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_SENDLOWATER,
                                 options.minimumSendBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.minimumReceiveBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_RECEIVELOWATER,
                                 options.minimumReceiveBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.sendTimeout().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_SENDTIMEOUT,
                                 options.sendTimeout().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.receiveTimeout().isNull()) {
        const int rc = setOption(handle,
                                 SocketOptUtil::k_SOCKETLEVEL,
                                 SocketOptUtil::k_RECEIVETIMEOUT,
                                 options.receiveTimeout().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.tcpNoDelay().isNull()) {
        const int rc = setOption(
                               handle,
                               SocketOptUtil::k_TCPLEVEL,
                               SocketOptUtil::k_TCPNODELAY,
                               static_cast<int>(options.tcpNoDelay().value()));
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    return 0;
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
