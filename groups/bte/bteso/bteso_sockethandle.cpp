// bteso_sockethandle.cpp          -*-C++-*-
#include <bteso_sockethandle.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_sockethandle_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CLASS DATA

#ifdef BTESO_PLATFORM_WIN_SOCKETS

const bteso_SocketHandle::Handle bteso_SocketHandle::INVALID_SOCKET_HANDLE =
                                                                INVALID_SOCKET;

#else

const bteso_SocketHandle::Handle bteso_SocketHandle::INVALID_SOCKET_HANDLE =
                                                                            -1;

#endif

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
