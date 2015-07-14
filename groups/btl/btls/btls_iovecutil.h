// btls_iovecutil.h              -*-C++-*-
#ifndef INCLUDED_BTLS_IOVECUTIL
#define INCLUDED_BTLS_IOVECUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")



//@PURPOSE: Provide utilities to manipulate iovec structures.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btls::IovecUtil: namespace for iovec manipulation utilities
//
//@SEE_ALSO: btls_iovec
//
//@DESCRIPTION:  This component provides a namespace for several utilities for
// accessing and manipulating sequences of 'btls::Iovec' and 'btls::Ovec'
// buffers, including scatter (respectively gather) a single continuous buffer
// into (respectively from) a sequence of buffers, computing the total length
// of a sequence of buffers, and rewriting the data from a sequence of buffers
// into a 'bdlmca::PooledBufferChain' or a 'bdlmca::Blob' object.
//
///Thread-safety
///-------------
// It is safe to invoke methods of this utility with distinct instances of
// their arguments in different threads.  It is safe to access a given
// sequence of buffers, 'bdlmca::Blob' or 'bdlmca::PooledBufferChain' object,
// within different methods of this utility from different threads, if no
// method modifies the contents of this object.  It is not safe to access or
// modify a given sequence of buffer, 'bdlmca::Blob' or 'bdlmca::PooledBufferChain'
// object from any method of this utility, while a method modifies the content
// of this object.
//
///Performance
///-----------
// Computing 'length' takes time proportional to the number of buffers in the
// sequence.  All other methods take time proportional to the total length of
// the buffers in the sequence.
//
///Usage
///-----
//..
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

                            // ====================
                            // class btls::IovecUtil
                            // ====================

namespace BloombergLP {

namespace bdlmca { class PooledBufferChain; }
namespace bdlmca { class PooledBufferChainFactory; }

namespace bdlmca { class Blob; }
namespace bdlmca { class BlobBufferFactory; }

namespace btls {
struct IovecUtil {
    // This 'struct' provides a namespace for several utilities for
    // manipulating 'Iovecs', including scatter/gather from a single continuous
    // buffer into an iovec structure, computing the length of an iovec
    // structure, and rewriting the data in an iovec structure into a
    // 'bdlmca::PooledBufferChain' or a 'bdlmca::Blob'.

    static void appendToBlob(bdlmca::Blob       *blob,
                             const Iovec *buffers,
                             int               numBuffers,
                             int               offset = 0);
    static void appendToBlob(bdlmca::Blob       *blob,
                             const Ovec  *buffers,
                             int               numBuffers,
                             int               offset = 0);
        // Append to the specified 'blob' the data from the specified sequence
        // of 'buffers' of the specified 'numBuffers' length.  Optionally
        // specify an 'offset' (in bytes) within the data contained in the
        // 'buffers' to indicate the start of the appended data; if offset is
        // not specified, then start at offset 0 (i.e., use the entire
        // 'buffers').  Upon return, the length of the 'blob' has been
        // increased by the number of characters appended, and the data
        // contained in the 'blob' is a concatenation of the data previously
        // held followed by the data contained in the buffers from the position
        // 'offset' until the end.  The behavior is undefined if 'offset' is
        // negative or is greater or equal than the total size of the specified
        // 'buffers', or if the total size of 'blob' is less than what is
        // needed and 'blob' was created without a blob buffer factory.

    static bdlmca::Blob *blob(const Iovec        *buffers,
                            int                      numBuffers,
                            int                      offset,
                            bdlmca::BlobBufferFactory *factory,
                            bslma::Allocator        *allocator);
    static bdlmca::Blob *blob(const Iovec        *buffers,
                            int                      numBuffers,
                            bdlmca::BlobBufferFactory *factory,
                            bslma::Allocator        *allocator);
        // Create a 'bdlmca::Blob' object allocated using the specified 'factory'
        // and containing data from the specified sequence of 'buffers' of the
        // specified 'numBuffers' length.  Optionally specify an 'offset' (in
        // bytes) within the data contained in the 'buffers' to indicate the
        // start of the copied data; if offset is not specified, then start at
        // offset 0 (i.e., use the entire 'buffers').  Return a pointer to the
        // created blob.  The behavior is undefined if 'offset' is negative or
        // if 'offset' is greater or equal than the total size of the
        // 'buffers'.

    static bdlmca::PooledBufferChain *chain(const Iovec *buffers,
                                          int               numBuffers,
                            //------------^
                            bdlmca::PooledBufferChainFactory *factory);
    static bdlmca::PooledBufferChain *chain(const Iovec *buffers,
                                          int               numBuffers,
                                          int               offset,
                            //------------^
                            bdlmca::PooledBufferChainFactory *factory);
        // Create a 'bdlmca::PooledBufferChain' object allocated using the
        // specified 'factory' and containing data from the specified sequence
        // of 'buffers' of the specified 'numBuffers' length.  Optionally
        // specify an 'offset' (in bytes) within the data contained in the
        // 'buffers' to indicate the start of the copied data; if 'offset' is
        // not specified, then start at offset 0 (i.e., use the entire
        // 'buffers').  Return a pointer to the created chain.  The behavior is
        // undefined if 'offset' is negative or if 'offset' is greater or equal
        // than the total size of the 'buffers'.

    static int gather(char             *buffer,
                      int               length,
                      const Iovec *buffers,
                      int               numBuffers);
    static int gather(char             *buffer,
                      int               length,
                      const Ovec  *buffers,
                      int               numBuffers);
        // Copy into the specified 'buffer' of the specified 'length', the
        // contents of each of the buffers in the specified sequence of
        // 'buffers' of the specified 'numBuffers' length.   Return the total
        // number of bytes copied.

    static int length(const Iovec *buffers, int numBuffers);
    static int length(const Ovec  *buffers, int numBuffers);
        // Return the total length in bytes of the specified sequence of
        // 'buffers' of the specified 'numBuffers' length.  The behavior is
        // undefined unless 'buffers' is not 0 and '0 <= numBuffers'.

    static void pivot(int              *bufferIdx,
                      int              *offset,
                      const Iovec *buffers,
                      int               numBuffers,
                      int               length);
    static void pivot(int              *bufferIdx,
                      int              *offset,
                      const Ovec  *buffers,
                      int               numBuffers,
                      int               index);
        // Load into the specified 'bufferIdx' the index of the buffer
        // and into the specified 'offset' the offset within this
        // buffer of the byte located at the specified global 'index'.
        // The behavior is undefined unless 'bufferIdx' is not 0,
        // 'offset' is not 0, 'buffers' is not 0, 0 <= numBuffers
        // and 0 <= index.  Note that if 'index' is greater than the total
        // length of the 'buffers', 'bufferIdx' is set to 'numBuffers', and the
        // 'offset' is not changed.

    static int scatter(const Iovec *buffers,
                       int               numBuffers,
                       const char       *buffer,
                       int               length);
    static int scatter(const Ovec  *buffers,
                       int               numBuffers,
                       const char       *buffer,
                       int               length);
        // Copy into the specified sequence of 'buffers' of the specified
        // 'numBuffers' length the data from the specified 'buffer' of the
        // specified 'length'.  Return the total number of bytes copied.
        // The behavior is undefined unless 'buffers' is not 0,
        // 0 <= numBuffers, 'buffer' is not 0 and 0 <= length.
};
}  // close package namespace

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
