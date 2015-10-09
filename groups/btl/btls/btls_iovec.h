// btls_iovec.h                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS_IOVEC
#define INCLUDED_BTLS_IOVEC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent data structures for scatter/gather IO.
//
//@CLASSES:
//  btls::Iovec: Data structure for scatter/gather read or write operations.
//  btls::Ovec: Data structure for scatter/gather write operations.
//
//@DESCRIPTION: This component provides platform-independent data structures
// for scatter/gather IO, each implemented as a structure that contains a
// pointer to a data buffer and the buffer's length.  The structures contain
// platform dependent fields, which must only be modified and accessed via the
// manipulators and accessors.  These structures hold but do not own the
// pointer to the data buffer and therefore will not allocate or deallocate the
// data buffer.
//
// For operations that modify the contents of the data buffer 'btls::Iovec'
// should be used.  This includes scatter/gather read or receive operations.
//
// For operations that do not modify the contents of the data buffer either
// 'btls::Iovec' or 'btls::Ovec' may be used.  This includes scatter/gather
// write or send operations.
//
///Usage
///-----
// The following snippets of code illustrate how to use 'btls::Iovec' with
// platform dependent scatter/gather operations.  Typically, an array of
// structures is created with each element containing a pointer to a buffer.  A
// pointer to the array of 'btls::Ovec' or 'btls::Iovec' is passed to the IO
// operation.  In this example an array of 3 buffer pointers is created.
//..
//  char buf1[10];
//  char buf2[4];
//  char buf3[7];
//  btls::Ovec vector[3];
//  vector[0].setBuffer(buf1, 10);
//  vector[1].setBuffer(buf2, 4);
//  vector[2].setBuffer(buf3, 7);
//..
// On UNIX-like systems the internal structure of 'btls::Iovec' and
// 'btls::Ovec' use the 'iovec' 'struct'.  This structure is used for the
// 'writev' and 'readv' scatter/gather read/write operations or within the
// 'msgbuf' 'struct' used by 'sendmsg' and 'readmsg' socket operations.
//
// On Windows the internal structure of 'btls::Iovec' and 'btls::Ovec' use the
// 'WSABUF' 'struct'.  This structure is used for the 'WSARecv' and 'WSASend'
// scatter/gather send/receive socket operations or within the 'WSAMSG'
// 'struct' used by 'WSARecvMsg' and 'WSASendMsg' socket operations.
//..
//  #ifndef BSLS_PLATFORM_CMP_MSVC
//      // Verify values for UNIX like systems
//      int socket = 0;
//      ::writev(socket, (struct iovec *) vector, 3);
//  #else
//      // Verify values for Windows
//      DWORD writeCount = 0;
//      SOCKET socket = (SOCKET) 0;
//      int ret =
//             TestWSASend(socket, (WSABUF *) vector, 3, &writeCount, 0, 0, 0);
//  #endif
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC
#if defined(BSLS_PLATFORM_CMP_MSVC) && !defined(_WINSOCK2API_)
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>                    // for WSABUF
    #define INCLUDED_WINSOCK2
    #endif
#endif
#else
    #ifndef INCLUDED_UNISTD
    #include <unistd.h>                      // for struct iovec
    #define INCLUDED_UNISTD
    #endif

    #ifndef INCLUDED_UIO_H
    #include <sys/uio.h>
    #define INCLUDED_UIO_H
    #endif
#endif

                               // ===========
                               // class Iovec
                               // ===========

namespace BloombergLP {
namespace btls {

class Ovec;

class Iovec {
    // This class provides a platform-independent data structure for
    // scatter/gather IO implemented as a structure that contains a pointer to
    // a data buffer and the buffer's length.  The implementation of this
    // structure contains platform dependent fields and therefore must only be
    // modified and accessed via the manipulators and accessors.  This class
    // holds but does not own the pointer to the data buffer and therefore will
    // not allocate or deallocate the data buffer.  'Iovec' may be used for
    // operations that modify the contents of the data buffer.  This includes
    // scatter/gather read or receive operations in addition to write or send
    // operations which do not modify the data buffer.

#ifdef BSLS_PLATFORM_CMP_MSVC
    // For Windows (Winsock2) platforms only.
    typedef WSABUF BUFFER;
#else
    // For UNIX-like platforms only.
    typedef struct iovec BUFFER;
#endif

    // DATA
    BUFFER d_buffer;

    // FRIENDS
    friend class Ovec;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Iovec, bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Iovec(void *buffer, int length);
        // Create an 'Iovec' with a pointer to a writeable buffer with the
        // specified 'buffer' and 'length'.

    Iovec(const Iovec& original);
        // Create an 'Iovec' object having the value of the specified
        // 'original'.

    Iovec();
        // Create an 'Iovec' where the values of the buffer and length are
        // undefined.

    ~Iovec();

    // MANIPULATORS
    Iovec& operator=(const Iovec& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    void setBuffer(void *buffer, int length);
        // Replace the pointer to and the length of the writable buffer with
        // the specified 'buffer' and 'length'.

    // ACCESSORS
    void *buffer() const;
        // Return a pointer to the writable buffer.

    int length() const;
        // Return the length of the writable buffer.
};

                                // ==========
                                // class Ovec
                                // ==========

class Ovec {
    // This class provides a platform-independent data structure for
    // scatter/gather IO implemented as a structure that contains a pointer to
    // a data buffer and the buffer's length.  The implementation of this
    // structure contains platform dependent fields and therefore must only be
    // modified and accessed via the manipulators and accessors.  This class
    // holds but does not own the pointer to the data buffer and therefore will
    // not allocate or deallocate the data buffer.  'Ovec' may only be used for
    // operations that do not modify the contents of the data buffer.  This
    // includes scatter/gather write or send operations.

    // DATA
    Iovec::BUFFER d_buffer;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Ovec, bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Ovec(const void *buffer, int length);
        // Create an 'Ovec' with a pointer to a non writeable buffer with the
        // specified 'buffer' and 'length'.

    Ovec(const Ovec & original);
        // Create an 'Ovec' object having the value of the specified
        // 'original'.

    Ovec(const Iovec & original);
        // Create an 'Ovec' object having the value of the specified 'original'
        // 'Iovec'.

    Ovec();
        // Create an 'Ovec' where the values of the buffer and length are
        // undefined.

    ~Ovec();

    // MANIPULATORS
    Ovec& operator=(const Ovec& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    void setBuffer(const void *buffer, int length);
        // Replace the pointer to and the length of the non-writable buffer
        // with the specified 'buffer' and 'length'.

    // ACCESSORS
    const void *buffer() const;
        // Return a pointer to the non writable buffer.

    int length() const;
        // Return the length of the non writable buffer.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // -----------
                               // class Iovec
                               // -----------

// CREATORS
inline
Iovec::Iovec()
{
}

inline
Iovec::~Iovec()
{
}

inline
Iovec::Iovec(const Iovec& original)
{
    d_buffer = original.d_buffer;
}

inline
Iovec::Iovec(void *buffer, int length)
{
#if defined(BSLS_PLATFORM_CMP_MSVC)
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) buffer;
    d_buffer.iov_len = length;
#endif
}

// MANIPULATORS

inline
Iovec& Iovec::operator=(const Iovec& rhs)
{
    if (this != &rhs) {
        d_buffer = rhs.d_buffer;
    }
    return *this;
}

inline
void Iovec::setBuffer(void *buffer, int length)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) buffer;
    d_buffer.iov_len = length;
#endif
}

// ACCESSORS

inline
void * Iovec::buffer() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.buf;
#else
    return d_buffer.iov_base;
#endif
}

inline
int Iovec::length() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.len;
#else
    return (int)d_buffer.iov_len;
#endif
}

                                // ----------
                                // class Ovec
                                // ----------

// CREATORS
inline
Ovec::Ovec() { }

inline
Ovec::~Ovec() { }

inline
Ovec::Ovec(const Iovec& original)
{
    d_buffer = original.d_buffer;
}

inline
Ovec::Ovec(const Ovec& original)
{
    d_buffer = original.d_buffer;
}

inline
Ovec::Ovec(const void *buffer, int length)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = (void *) buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) const_cast<void *>(buffer);
    d_buffer.iov_len = length;
#endif
}

// MANIPULATORS
inline
Ovec& Ovec::operator=(const Ovec& rhs)
{
    if (this != &rhs) {
        d_buffer = rhs.d_buffer;
    }
    return *this;
}

inline
void Ovec::setBuffer(const void *buffer, int length)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = (void *) buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) const_cast<void *>(buffer);
    d_buffer.iov_len = length;
#endif
}

// ACCESSORS
inline
const void *Ovec::buffer() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.buf;
#else
    return d_buffer.iov_base;
#endif
}

inline
int Ovec::length() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.len;
#else
    return (int)d_buffer.iov_len;
#endif
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
