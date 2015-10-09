// btlso_inetstreamsocketfactory.h                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_INETSTREAMSOCKETFACTORY
#define INCLUDED_BTLSO_INETSTREAMSOCKETFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Implementation for TCP based stream sockets.
//
//@CLASSES:
//  btlso::InetStreamSocketFactory: factory for TCP-based stream-sockets
//
//@SEE_ALSO: btlso_inetstreamsocket, btlso_ipv4address
//
//@DESCRIPTION: This component implements a factory to allocate and deallocate
// them.  The stream sockets are of type 'btlso::InetStreamSocket<ADDRESS>'
// conforming to the 'btlso::StreamSocket<ADDRESS>' protocol.  The classes are
// templatized to provide type-safe address class specialization.  The only
// address type currently supported is IPv4 (as provided by 'btlso_ipv4address'
// component).  Therefore, the template parameter will always be
// 'btlso::IPv4Address'.  The factory,
// 'btlso::InetStreamSocketFactory<ADDRESS>', creates and destroys instances of
// the 'btlso::InetStreamSocket<ADDRESS>'.  Two interfaces are available for
// creation of stream sockets.  One does not take a socket handle creates a new
// socket in the default initial state when a new stream socket is allocated.
// The second takes the handle to an existing TCP-based stream socket and loads
// it into newly-allocated stream socket object.  In this case, no assumption
// is made about the state of the existing socket.  Every instance of
// 'btlso::InetStreamSocket<ADDRESS>' must be destroyed using the deallocate
// operation of 'btlso::InetStreamSocketFactory<ADDRESS>'.
//
// The creation of the socket factory provided by this component will enable
// socket operations (by calling 'btlso::SocketImpUtil::startup') method; the
// the destruction will disable socket operations (by calling
// 'btlso::SocketImpUtil::cleanup' method).
//
///Thread Safety
///-------------
// The classes provided by this component depend on a 'bslma::Allocator'
// instance to supply memory.  If the allocator is not thread enabled then the
// instances of this component that use the same allocator instance will
// consequently not be thread safe.  Otherwise, this component provides the
// following guarantees:
//
// The 'btlso::InetStreamSocketFactory' is *thread-enabled* and any thread can
// call any method *on the same instance* safely and the operations will
// proceed in parallel.  WARNING: simultaneous deallocation of the same
// instance of a stream socket may result in undefined behavior.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Create a New Stream Socket
///- - - - - - - - - - - - - - - - - - -
// We can use 'btlso::InetStreamSocketFactory' to allocate a new TCP-based
// stream socket.
//
// First, we create a 'btlso::InetStreamSocketFactory' object:
//..
//  btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Then, we create a stream socket:
//..
//  btlso::StreamSocket<btlso::IPv4Address> *mySocket = factory.allocate();
//  assert(mySocket);
//..
// 'mySocket' can now be used for TCP communication.
//
// Finally, when we're done, we recycle the socket:
//..
//  factory.deallocate(mySocket);
//..
//
///Example 2: Create a 'btlso::StreamSocket' Object From Existing Socket Handle
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use 'btlso::InetStreamSocketFactory' to allocate a
// 'btlso::StreamSocket' object that attaches to an existing socket handle.
// This socket handle may be created from a third-party library (such as
// OpenSSL).  Using a 'btlso::StreamSocket' object rather than the socket
// handle directly is highly desirable as it enables the use of other BTE
// components on the socket.  In this example, the socket handle is created
// from the 'btlso_socketimputil' component for illustrative purpose.
//
// First, we create a socket handle 'fd':
//..
//  btlso::SocketHandle::Handle fd;
//  int                         nativeErrNo = 0;
//
//  btlso::SocketImpUtil::open<btlso::IPv4Address>(
//                                       &fd,
//                                       btlso::SocketImpUtil::k_SOCKET_STREAM,
//                                       &nativeErrNo);
//  assert(0 == nativeErrNo);
//..
// Then, we create factory:
//..
//  btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Next, we allocate a stream socket attached to 'fd':
//..
//  btlso::StreamSocket<btlso::IPv4Address> *mySocket = factory.allocate(fd);
//  assert(mySocket);
//..
// Notice that 'fd' is passed into the 'allocate' method as an argument.  Any
// BTE component that uses 'btlso::StreamSocket<btlso::IPv4Address>' can now be
// used on 'mySocket'.
//
// Finally, when we're done, we recycle the socket:
//..
//  factory.deallocate(mySocket);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKETFACTORY
#include <btlso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#include <btlso_streamsocket.h>
#endif

#ifndef INCLUDED_BTLSO_INETSTREAMSOCKET
#include <btlso_inetstreamsocket.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETIMPUTIL
#include <btlso_socketimputil.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

namespace BloombergLP {

namespace btlso {
                    // =============================
                    // class InetStreamSocketFactory
                    // =============================

template <class ADDRESS>
class InetStreamSocketFactory : public StreamSocketFactory<ADDRESS> {
    // Provide an implementation for a factory to allocate and deallocate
    // TCP-based stream socket objects of type 'InetStreamSocket<ADDRESS>'.

    bslma::Allocator *d_allocator_p;       // held, not owned

  public:
    // CREATORS
    explicit InetStreamSocketFactory(bslma::Allocator *basicAllocator = 0);
        // Create a stream socket factory.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~InetStreamSocketFactory();
        // Destroy this factory.  The stream sockets created by this factory
        // will not be destroyed when this factory is destroyed.

    // MANIPULATORS
    virtual StreamSocket<ADDRESS> *allocate();
        // Create a stream-based socket of type 'InetStreamSocket<ADDRESS>'.
        // Return the address of the newly-created socket on success, and 0
        // otherwise.

    virtual StreamSocket<ADDRESS> *allocate(SocketHandle::Handle handle);
        // Create a stream-based socket of type 'InetStreamSocket<ADDRESS>'
        // attached to the specified socket 'handle'.  Return the address of
        // the newly-created socket on success, and 0 otherwise.  The behavior
        // is undefined unless 'handle' refers to a valid TCP-based stream
        // socket with the address class 'ADDRESS'.

    virtual void deallocate(StreamSocket<ADDRESS> *socket);
        // Return the specified 'socket' back to this factory.  The behavior is
        // undefined unless 'socket' was allocated using this factory or was
        // created through an 'accept' from a stream socket created using this
        // factory, and has not already been deallocated.  Note that the
        // underlying TCP-based stream socket will be closed.

    virtual void deallocate(StreamSocket<ADDRESS> *socket,
                            bool                   closeHandleFlag);
        // Return the specified 'socket' back to this factory and, if the
        // specified 'closeHandleFlag' is 'true', also close the underlying
        // TCP-based stream socket.  If 'closeHandleFlag' is 'false', then
        // ownership of the underlying TCP-based stream socket is transferred
        // to the caller and the TCP-based socket is left open.  The behavior
        // is undefined unless 'socket' was allocated using this factory or was
        // created through an 'accept' from a stream socket created using this
        // factory, and has not already been deallocated.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                    // -----------------------------
                    // class InetStreamSocketFactory
                    // -----------------------------

// CREATORS
template <class ADDRESS>
inline
InetStreamSocketFactory<ADDRESS>::InetStreamSocketFactory(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    SocketImpUtil::startup();
}

template <class ADDRESS>
inline
InetStreamSocketFactory<ADDRESS>::~InetStreamSocketFactory()
{
    SocketImpUtil::cleanup();
}

// MANIPULATORS
template <class ADDRESS>
StreamSocket<ADDRESS> *InetStreamSocketFactory<ADDRESS>::allocate()
{
    SocketHandle::Handle newSocketHandle;

    int ret = SocketImpUtil::open<ADDRESS>(&newSocketHandle,
                                           SocketImpUtil::k_SOCKET_STREAM);

    if (ret < 0) {
        return 0;                                                     // RETURN
    }

    btlso::InetStreamSocketCloseGuard autoDeallocate(newSocketHandle);

    // On successfully opening a new socket, create a new 'InetStreamSocket'
    // for this socket.

    StreamSocket<ADDRESS> *newStreamSocket =
                new (*d_allocator_p) InetStreamSocket<ADDRESS>(newSocketHandle,
                                                               d_allocator_p);

    autoDeallocate.release();

    return newStreamSocket;
}

template <class ADDRESS>
StreamSocket<ADDRESS> *InetStreamSocketFactory<ADDRESS>::allocate(
                                             SocketHandle::Handle socketHandle)
{
    // Create a new 'InetStreamSocket' for this socket.

    return new (*d_allocator_p) InetStreamSocket<ADDRESS>(socketHandle,
                                                          d_allocator_p);
}

template <class ADDRESS>
void InetStreamSocketFactory<ADDRESS>::deallocate(
                                                 StreamSocket<ADDRESS> *socket)
{
    SocketImpUtil::close(socket->handle());

    dynamic_cast<InetStreamSocket<ADDRESS> *>(socket)->
                                                  ~InetStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

template <class ADDRESS>
void InetStreamSocketFactory<ADDRESS>::deallocate(
                                        StreamSocket<ADDRESS> *socket,
                                        bool                   closeHandleFlag)
{
    if (closeHandleFlag) {
        SocketImpUtil::close(socket->handle());
    }

    dynamic_cast<InetStreamSocket<ADDRESS> *>(socket)->
                                                  ~InetStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

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
