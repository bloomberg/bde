// btlso_streamsocketfactory.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_STREAMSOCKETFACTORY
#define INCLUDED_BTLSO_STREAMSOCKETFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide protocol for a factory producing stream sockets.
//
//@CLASSES:
//  btlso::StreamSocketFactory: protocol for factory of stream socket objects
//  btlso::StreamSocketFactoryAutoDeallocateGuard: socket deallocate guard
//  btlso::StreamSocketFactoryDeleter: stream socket deleter
//
//@SEE_ALSO: btlso_streamsocket
//
//@DESCRIPTION: This component defines a family of abstract interfaces for
// factories, providing allocation and deallocation operations for stream-
// based sockets.  The protocol itself is generic, parameterized by address
// type.  A concrete instantiation of this interface for a particular address
// defines the factory protocol for all concrete implementations of stream
// socket factories for this address type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Allocating Stream Sockets using a Stream Socket Factory
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code demonstrate how to use a socket factory for
// allocating sockets of IPv4 address type.  We assume that there is a function
// 'f' that needs to use IPv4 sockets.  This function will get an address of
// the stream socket factory for the IPv4 address type and allocate sockets
// from it:
//..
//  int f(btlso::StreamSocketFactory<btlso::IPv4Address> *factory) {
//..
//
// Now, allocate a stream socket:
//..
//      btlso::StreamSocket<btlso::IPv4Address> *conn = factory->allocate();
//      if (!conn) {
//          bsl::cout << "Socket cannot be allocated." << bsl::endl;
//          return -1;
//      }
//..
// At this point, the allocated 'conn' can be used for communication.  The
// stream socket is returned to the factory when we're done:
//..
//      // ...
//
//      factory->deallocate(conn);
//      return 0;
//  }
//..
//
///Example 2: Binding a Stream Socket to a 'bslma::ManagedPtr'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// It is occasionally necessary to create smart pointers to manage a
// 'btlso::StreamSocket' object.  The example below shows the syntax for
// constructing managed pointer object for a stream socket using
// 'btlso::StreamSocketFactoryDeleter'.  This example assumes that a concrete
// 'btlso::StreamSocketFactory' named 'factory' is available and can be used to
// allocate stream socket objects.
//
// First, we allocate a stream socket:
//..
//  btlso::StreamSocket<btlso::IPv4Address> *sa  = factory->allocate();
//..
// Then, we construct a managed pointer to a stream socket:
//..
//  typedef btlso::StreamSocketFactoryDeleter Deleter;
//
//  bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
//                    saManagedPtr(sa,
//                                 factory,
//                                 &Deleter::deleteObject<btlso::IPv4Address>);
//..
// See 'btlso_streamsocket' for more in-depth usage examples.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

namespace BloombergLP {

namespace btlso {

template <class ADDRESS> class StreamSocket;

                        // =========================
                        // class StreamSocketFactory
                        // =========================

template <class ADDRESS>
class StreamSocketFactory {
    // This class represents a family of protocols for a stream-based socket
    // factories.  The template parameter identifies of the address type (e.g.,
    // 'IPv4Address' for IPv4 addresses) with no assumptions on operations.

  public:
    // CREATORS
    virtual ~StreamSocketFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual StreamSocket<ADDRESS> *allocate() = 0;
        // Create a stream-based socket; return its address on success, and 0
        // otherwise.

    virtual void deallocate(StreamSocket<ADDRESS> *streamSocket) = 0;
        // Deallocate the specified 'streamSocket', terminating all operations
        // on the underlying system socket.  The behavior is undefined unless
        // 'streamSocket' was allocated using this factory (or created through
        // an 'accept' operation on a stream socket allocated using this
        // factory) and has not since been deallocated.
};

             // ============================================
             // class StreamSocketFactoryAutoDeallocateGuard
             // ============================================

template <class ADDRESS>
class StreamSocketFactoryAutoDeallocateGuard {
    // This class implements a proctor for a socket, using the specified
    // factory to deallocate it upon destruction unless this socket has been
    // released from management by this proctor prior to destruction.

    // DATA
    union {
        StreamSocket<ADDRESS>     *d_socket_p;
        StreamSocket<ADDRESS>    **d_socketPtr_p;
    } d_arena;

    StreamSocketFactory<ADDRESS>  *d_deleter_p;
    bool                           d_resetFlag;

    // PRIVATE ACCESSORS
    StreamSocket<ADDRESS>* socket() const;
        // Return the socket currently under management by this proctor.

  public:
    // CREATORS
    StreamSocketFactoryAutoDeallocateGuard(
                                        StreamSocket<ADDRESS>        *socket,
                                        StreamSocketFactory<ADDRESS> *factory);
        // Create a proctor for the specified 'socket', using the specified
        // 'factory' for deallocating upon destruction.

    StreamSocketFactoryAutoDeallocateGuard(
                                      StreamSocket<ADDRESS>        **socketPtr,
                                      StreamSocketFactory<ADDRESS>  *factory);
        // Create a proctor for the indirectly specified 'socketPtr', using the
        // specified 'factory' for deallocating upon destruction.  In addition,
        // upon destruction, the pointer '*socketPtr' is reset to 0.

    ~StreamSocketFactoryAutoDeallocateGuard();
        // Deallocate the proctored socket, unless released from management by
        // this proctor.

    // MANIPULATORS
    void release();
        // Release the proctored socket from management by this proctor.
};

             // ================================
             // class StreamSocketFactoryDeleter
             // ================================

struct StreamSocketFactoryDeleter {
    // This 'struct' implements a 'deleter' function, 'deleteObject', for
    // destroying stream socket objects using a specified 'factory'.  The
    // method 'deleteObject' matches the signature of a factory function that
    // can be supplied to 'bslma::ManagedPtr'.

    // CLASS METHODS
    template <class ADDRESS>
    static void deleteObject(void *socket, void *factory);
        // Deallocate the specified 'socket' using the specified 'factory'.
        // The behavior is undefined unless 'socket' was allocated using
        // 'factory'.  Note that this function matches the signature of a
        // factory function that can be supplied to 'bslma::ManagedPtr', and is
        // intended to simplify the creation of managed-pointers to
        // 'StreamSocket' objects.
};

// ============================================================================
//                         INLINE FUNCTIONS DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class StreamSocketFactory
                        // -------------------------

// CREATORS
template <class ADDRESS>
inline
StreamSocketFactory<ADDRESS>::~StreamSocketFactory()
{
    // Implementation note: destructor is inlined to avoid template repository.
}

             // --------------------------------------------
             // class StreamSocketFactoryAutoDeallocateGuard
             // --------------------------------------------

// PRIVATE ACCESSORS
template <class ADDRESS>
inline
StreamSocket<ADDRESS> *
StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::socket() const
{
    if (d_resetFlag) {
        return *d_arena.d_socketPtr_p;                                // RETURN
    }
    return d_arena.d_socket_p;
}

// CREATORS
template <class ADDRESS>
inline
StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
StreamSocketFactoryAutoDeallocateGuard(StreamSocket<ADDRESS>        *socket,
                                       StreamSocketFactory<ADDRESS> *factory)
: d_deleter_p(factory)
, d_resetFlag(false)
{
    d_arena.d_socket_p = socket;
}

template <class ADDRESS>
inline
StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
StreamSocketFactoryAutoDeallocateGuard(StreamSocket<ADDRESS>        **socket,
                                       StreamSocketFactory<ADDRESS>  *factory)
: d_deleter_p(factory)
, d_resetFlag(true)
{
    d_arena.d_socketPtr_p = socket;
}

template <class ADDRESS>
inline
StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
~StreamSocketFactoryAutoDeallocateGuard()
{
    if (socket()) {
        d_deleter_p->deallocate(socket());
        if (d_resetFlag) {
            *d_arena.d_socketPtr_p = 0;
        }
    }
}

// MANIPULATORS
template <class ADDRESS>
inline
void StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::release()
{
    d_arena.d_socket_p = 0;
    d_resetFlag = false;
}

                        // --------------------------------
                        // class StreamSocketFactoryDeleter
                        // --------------------------------

template <class ADDRESS>
void StreamSocketFactoryDeleter::deleteObject(void *socket, void *factory)
{
    typedef btlso::StreamSocket<ADDRESS>        StreamSocket;
    typedef btlso::StreamSocketFactory<ADDRESS> StreamSocketFactory;

    StreamSocket        *streamSocketPtr = static_cast<StreamSocket *>(socket);
    StreamSocketFactory *socketFactoryPtr =
                                   static_cast<StreamSocketFactory *>(factory);

    socketFactoryPtr->deallocate(streamSocketPtr);
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
