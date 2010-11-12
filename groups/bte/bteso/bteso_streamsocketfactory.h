// bteso_streamsocketfactory.h       -*-C++-*-
#ifndef INCLUDED_BTESO_STREAMSOCKETFACTORY
#define INCLUDED_BTESO_STREAMSOCKETFACTORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide protocol for a factory producing stream sockets.
//
//@CLASSES:
// bteso_StreamSocketFactory: protocol for factory of stream socket objects
//
//@SEE_ALSO: bteso_streamsocket
//
//@AUTHOR: Paul Staniforth (pstaniforth)
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
// The following snippets of code demonstrate how to use a socket factory for
// allocating sockets of IPv4 address type.  We assume that there is a
// function 'f' that needs to use IPv4 sockets.  This function will get an
// address of the stream socket factory for the IPv4 address type and allocate
// sockets from it:
//..
//  int f(bteso_StreamSocketFactory<bteso_IPv4Address> *factory) {
//..
//
// Now, allocate a stream socket:
//..
//      bteso_StreamSocket<bteso_IPv4Address> *connection =
//          factory->allocate();
//      if (!connection) {
//          bsl::cout << "Socket cannot be allocated." << bsl::endl;
//          return -1;
//      }
//..
// At this point, the allocated 'connection' can be used for communication.
// The stream socket is returned to the factory when we're done:
//..
//      // ...
//
//      factory->deallocate(connection);
//      return 0;
//  }
//..
// See 'bteso_streamsocket' for in-depth usage examples.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

namespace BloombergLP {

template <class ADDRESS> class bteso_StreamSocket;

                        // ===============================
                        // class bteso_StreamSocketFactory
                        // ===============================

template <class ADDRESS>
class bteso_StreamSocketFactory {
    // This class represents a family of protocols  for a stream-based socket
    // factories.  The template parameter identifies of the address type
    // (e.g., 'bteso_IPv4Address' for IPv4 addresses) with no assumptions on
    // operations.

  public:
    // CREATORS
    virtual ~bteso_StreamSocketFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual bteso_StreamSocket<ADDRESS> *allocate() = 0;
        // Create a stream-based socket; return its address on success,
        // and 0 otherwise.

    virtual void deallocate(bteso_StreamSocket<ADDRESS> *streamSocket) = 0;
        // Deallocate the specified 'streamSocket', terminating all operations
        // on the underlying system socket.   The behavior is undefined unless
        // 'streamSocket' was allocated using this factory (or created through
        // an 'accept' operation on a stream socket allocated using this
        // factory) and has not since been deallocated.
};

             // ==================================================
             // class bteso_StreamSocketFactoryAutoDeallocateGuard
             // ==================================================

template <class ADDRESS>
class bteso_StreamSocketFactoryAutoDeallocateGuard {
    // This class implements a proctor for a socket, using the specified
    // factory to deallocate it upon destruction unless this socket has been
    // released from management by this proctor prior to destruction.

    // DATA
    union {
        bteso_StreamSocket<ADDRESS>          *d_socket_p;
        bteso_StreamSocket<ADDRESS>         **d_socketPtr_p;
    } d_arena;
    bteso_StreamSocketFactory<ADDRESS>       *d_deleter_p;
    bool                                      d_resetFlag;

    // PRIVATE ACCESSORS
    bteso_StreamSocket<ADDRESS>* socket() const;
        // Return the socket currently under management by this proctor.

  public:
    // CREATORS
    bteso_StreamSocketFactoryAutoDeallocateGuard(
                                  bteso_StreamSocket<ADDRESS>        *socket,
                                  bteso_StreamSocketFactory<ADDRESS> *factory);
        // Create a proctor for the specified 'socket', using the specified
        // 'factory' for deallocating upon destruction.

    bteso_StreamSocketFactoryAutoDeallocateGuard(
                                bteso_StreamSocket<ADDRESS>        **socketPtr,
                                bteso_StreamSocketFactory<ADDRESS>  *factory);
        // Create a proctor for the indirectly specified 'socketPtr', using the
        // specified 'factory' for deallocating upon destruction.  In addition,
        // upon destruction, the pointer '*socketPtr' is reset to 0.

    ~bteso_StreamSocketFactoryAutoDeallocateGuard();
        // Deallocate the proctored socket, unless released from management by
        // this proctor.

    // MANIPULATORS
    void release();
        // Release the proctored socket from management by this proctor.
};

// ===========================================================================
//                         INLINE FUNCTIONS DEFINITIONS
// ===========================================================================

                        // -------------------------------
                        // class bteso_StreamSocketFactory
                        // -------------------------------

// CREATORS
template <class ADDRESS>
inline
bteso_StreamSocketFactory<ADDRESS>::~bteso_StreamSocketFactory()
{
}

// Implementation note: destructor is inlined to avoid template repository.

             // --------------------------------------------------
             // class bteso_StreamSocketFactoryAutoDeallocateGuard
             // --------------------------------------------------

// PRIVATE ACCESSORS
template <class ADDRESS>
inline
bteso_StreamSocket<ADDRESS> *
bteso_StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::socket() const
{
    if (d_resetFlag) {
        return *d_arena.d_socketPtr_p;
    }
    return d_arena.d_socket_p;
}


// CREATORS
template <class ADDRESS>
inline
bteso_StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
bteso_StreamSocketFactoryAutoDeallocateGuard(
                                   bteso_StreamSocket<ADDRESS>        *socket,
                                   bteso_StreamSocketFactory<ADDRESS> *factory)
: d_deleter_p(factory)
, d_resetFlag(false)
{
    d_arena.d_socket_p = socket;
}

template <class ADDRESS>
inline
bteso_StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
bteso_StreamSocketFactoryAutoDeallocateGuard(
                                bteso_StreamSocket<ADDRESS>        **socketPtr,
                                bteso_StreamSocketFactory<ADDRESS>  *factory)
: d_deleter_p(factory)
, d_resetFlag(true)
{
    d_arena.d_socketPtr_p = socketPtr;
}

template <class ADDRESS>
inline
bteso_StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::
~bteso_StreamSocketFactoryAutoDeallocateGuard()
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
void bteso_StreamSocketFactoryAutoDeallocateGuard<ADDRESS>::release()
{
    d_arena.d_socket_p = 0;
    d_resetFlag = false;
}

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
