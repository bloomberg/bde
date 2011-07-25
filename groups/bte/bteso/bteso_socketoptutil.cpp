// bteso_socketoptutil.cpp                                            -*-C++-*-
#include <bteso_socketoptutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_socketoptutil_cpp,"$Id$ $CSID$")

#include <bteso_socketoptions.h>
#include <bteso_lingeroptions.h>

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_socketimputil.h>                // for testing only
#include <bteso_ipv4address.h>                  // for testing only
#endif

namespace BloombergLP {

int bteso_SocketOptUtil::setSocketOptions(bteso_SocketHandle::Handle handle,
                                          const bteso_SocketOptions& options)
{
    if (!options.debugFlag().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_DEBUGINFO,
                                 (int) options.debugFlag().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.allowBroadcasting().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_BROADCAST,
                                 (int) options.allowBroadcasting().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.reuseAddress().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                 (int) options.reuseAddress().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.keepAlive().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_KEEPALIVE,
                                 (int) options.keepAlive().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.bypassNormalRouting().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_DONTROUTE,
                                 (int) options.bypassNormalRouting().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.linger().isNull()) {
        const bteso_LingerOptions&      lingerOptions =
                                                     options.linger().value();
        bteso_SocketOptUtil::LingerData lingerData;

        lingerData.l_onoff  = lingerOptions.lingerFlag();
        lingerData.l_linger = lingerOptions.timeout();

        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_LINGER,
                                 lingerData);
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        const int rc = setOption(
                              handle,
                              bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                              bteso_SocketOptUtil::BTESO_OOBINLINE,
                             (int) options.leaveOutOfBandDataInline().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.sendBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                 options.sendBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.receiveBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                                 options.receiveBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.minimumSendBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_SENDLOWATER,
                                 options.minimumSendBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.minimumReceiveBufferSize().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_RECEIVELOWATER,
                                 options.minimumReceiveBufferSize().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.sendTimeout().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_SENDTIMEOUT,
                                 options.sendTimeout().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    if (!options.receiveTimeout().isNull()) {
        const int rc = setOption(handle,
                                 bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                 bteso_SocketOptUtil::BTESO_RECEIVETIMEOUT,
                                 options.receiveTimeout().value());
        if (rc) {
            return rc;                                                // RETURN
        }
    }

    return 0;
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
