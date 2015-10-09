// btlso_sockethandle.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_sockethandle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_sockethandle_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                        // ------------------
                        // class SocketHandle
                        // ------------------

// CLASS DATA

#ifdef BTLSO_PLATFORM_WIN_SOCKETS

const btlso::SocketHandle::Handle
btlso::SocketHandle::INVALID_SOCKET_HANDLE = INVALID_SOCKET;

#else

const btlso::SocketHandle::Handle
btlso::SocketHandle::INVALID_SOCKET_HANDLE = -1;

#endif

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
