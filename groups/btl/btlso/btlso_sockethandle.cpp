// btlso_sockethandle.cpp          -*-C++-*-
#include <btlso_sockethandle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_sockethandle_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CLASS DATA

#ifdef BTLSO_PLATFORM_WIN_SOCKETS

const btlso::SocketHandle::Handle btlso::SocketHandle::INVALID_SOCKET_HANDLE =
                                                                INVALID_SOCKET;

#else

const btlso::SocketHandle::Handle btlso::SocketHandle::INVALID_SOCKET_HANDLE =
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
