// btes_iovec.h                 -*-C++-*-
#ifndef INCLUDED_BTES_IOVEC
#define INCLUDED_BTES_IOVEC

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide platform-independent data structures for scatter/gather IO.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btes_Iovec: Data structure for scatter/gather read or write operations.
//  btes_Ovec: Data structure for scatter/gather write operations.
//
//@DESCRIPTION: This component provides platform-independent data structures
// for scatter/gather IO, each implemented as a structure that contains a
// pointer to a data buffer and the buffer's length.  The structures contain
// platform dependent fields, which must only be modified and accessed via the
// manipulators and accessors.  These structures hold but do not own the
// pointer to the data buffer and therefore will not allocate or deallocate the
// data buffer.
//
// For operations that modify the contents of the data buffer
// 'btes_Iovec' should be used.  This includes scatter/gather read or
// receive operations.
//
// For operations that do not modify the contents of the data buffer either
// 'btes_Iovec' or 'btes_Ovec' may be used.  This includes
// scatter/gather write or send operations.
//
//USAGE
//-----
// The following snippets of code illustrate how to use 'btes_Iovec' with
// platform dependent scatter/gather operations.  Typically, an array of
// structures is created with each element containing a pointer to a buffer.
// A pointer to the array of 'btes_Ovec' or 'btes_Iovec' is
// passed to the IO operation.  In this example an array of 3 buffer pointers
// is created.
//..
// char buf1[10];
// char buf2[4];
// char buf3[7];
// btes_Ovec vector[3];
// vector[0].setBuffer(buf1, 10);
// vector[1].setBuffer(buf2, 4);
// vector[2].setBuffer(buf3, 7);
//..
// On UNIX-like systems the internal structure of 'btes_Iovec' and 'btes_Ovec'
// use the 'iovec' 'struct'.  This structure is used for the 'writev' and
// 'readv' scatter/gather read/write operations or within the 'msgbuf' 'struct'
// used by 'sendmsg' and 'readmsg' socket operations.
//..
// int writeCount = ::writev(socket, (struct iovec *) vector, 3);
//..
// On Windows the internal structure of 'btes_Iovec' and 'btes_Ovec' use the
// 'WSABUF' 'struct'.  This structure is used for the 'WSARecv' and 'WSASend'
// scatter/gather send/receive socket operations or within the 'WSAMSG'
// 'struct' used by 'WSARecvMsg' and 'WSASendMsg' socket operations.
//..
// int ret = ::WSASend(socket, (WSABUF *) vector, 3, &writeCount, 0, 0, 0);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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

    #ifndef INCLUDED_SYS_UIO
    #include <sys/uio.h>
    #define INCLUDED_SYS_UIO
    #endif
#endif


                        // ================
                        // class btes_Iovec
                        // ================

namespace BloombergLP {

class btes_Ovec;
class btes_Iovec {
    // This class provides a platform-independent data structure for
    // scatter/gather IO implemented as a structure that contains a pointer
    // to a data buffer and the buffer's length.  The implementation of
    // this structure contains platform dependent fields and therefore must
    // only be modified and accessed via the manipulators and accessors.
    // This class holds but does not own the pointer to the data buffer
    // and therefore will not allocate or deallocate the data buffer.
    // 'btes_Iovec' may be used for operations that modify the
    // contents of the data buffer.  This includes scatter/gather read or
    // receive operations in addition to write or send operations which do
    // not modify the data buffer.

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
    friend class btes_Ovec;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(btes_Iovec, bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    btes_Iovec(void *buffer, int length);
        // Create an 'btes_Iovec' with a pointer to a writeable buffer with the
        // specified 'buffer' and 'length'.

    btes_Iovec(const btes_Iovec& original);
        // Create an 'btes_Iovec' object having the value of the specified
        // 'original'.

    btes_Iovec();
        // Create an 'btes_Iovec' where the values of the buffer and length are
        // undefined.

    ~btes_Iovec();

    // MANIPULATORS
    btes_Iovec& operator=(const btes_Iovec& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    void setBuffer(void *buffer, int length);
        // Replace the pointer to and the length of the writable buffer
        // with the specified 'buffer' and 'length'.

    // ACCESSORS
    void *buffer() const;
        // Return a pointer to the writable buffer.

    int length() const;
        // Return the length of the writable buffer.
};

                        // ===============
                        // class btes_Ovec
                        // ===============

class btes_Ovec {
    // This class provides a platform-independent data structure for
    // scatter/gather IO implemented as a structure that contains a pointer
    // to a data buffer and the buffer's length.  The implementation of
    // this structure contains platform dependent fields and therefore must
    // only be modified and accessed via the manipulators and accessors.
    // This class holds but does not own the pointer to the data buffer
    // and therefore will not allocate or deallocate the data buffer.
    // 'btes_Ovec' may only be used for operations that do not
    // modify the contents of the data buffer.  This includes
    // scatter/gather write or send operations.

    // DATA
    btes_Iovec::BUFFER d_buffer;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(btes_Ovec, bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    btes_Ovec(const void *buffer, int length);
        // Create an 'btes_Ovec' with a pointer to a non writeable buffer with
        // the specified 'buffer' and 'length'.

    btes_Ovec(const btes_Ovec & original);
        // Create an 'btes_Ovec' object having the value of the specified
        // 'original'.

    btes_Ovec(const btes_Iovec & original);
        // Create an 'btes_Ovec' object having the value of the specified
        // 'original' 'btes_Iovec'.

    btes_Ovec();
        // Create an 'btes_Ovec' where the values of the buffer and length are
        // undefined.

    ~btes_Ovec();

    // MANIPULATORS
    btes_Ovec& operator=(const btes_Ovec& rhs);
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


// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------
                        // class btes_Iovec
                        // ----------------

// CREATORS
inline btes_Iovec::btes_Iovec()
{
}

inline btes_Iovec::~btes_Iovec()
{
}

inline btes_Iovec::btes_Iovec(const btes_Iovec& iovec)
{
    d_buffer = iovec.d_buffer;
}

inline btes_Iovec::btes_Iovec(void *buffer, int length)
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

inline btes_Iovec& btes_Iovec::operator=(const btes_Iovec& rhs)
{
    if (this != &rhs) {
        d_buffer = rhs.d_buffer;
    }
    return *this;
}

inline void btes_Iovec::setBuffer(void *buffer, int length)
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

inline void * btes_Iovec::buffer() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.buf;
#else
    return d_buffer.iov_base;
#endif
}

inline int btes_Iovec::length() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.len;
#else
    return (int)d_buffer.iov_len;
#endif
}

                        // ---------------
                        // class btes_Ovec
                        // ---------------


// CREATORS

inline btes_Ovec::btes_Ovec() { }

inline btes_Ovec::~btes_Ovec() { }

inline btes_Ovec::btes_Ovec(const btes_Iovec& iovec)
{
    d_buffer = iovec.d_buffer;
}

inline btes_Ovec::btes_Ovec(const btes_Ovec& iovec)
{
    d_buffer = iovec.d_buffer;
}

inline btes_Ovec::btes_Ovec(const void *buffer, int length)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = (void *) buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) buffer;
    d_buffer.iov_len = length;
#endif
}

// MANIPULATORS

inline btes_Ovec& btes_Ovec::operator=(const btes_Ovec& rhs)
{
    if (this != &rhs) {
        d_buffer = rhs.d_buffer;
    }
    return *this;
}

inline void btes_Ovec::setBuffer(const void *buffer, int length)
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    d_buffer.buf = (char FAR *) buffer;
    d_buffer.len = (u_long) length;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    d_buffer.iov_base = (void *) buffer;
    d_buffer.iov_len = length;
#else
    d_buffer.iov_base = (caddr_t) buffer;
    d_buffer.iov_len = length;
#endif
}

// ACCESSORS

inline const void *btes_Ovec::buffer() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.buf;
#else
    return d_buffer.iov_base;
#endif
}

inline int btes_Ovec::length() const
{
#ifdef BSLS_PLATFORM_CMP_MSVC
    return d_buffer.len;
#else
    return (int)d_buffer.iov_len;
#endif
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
